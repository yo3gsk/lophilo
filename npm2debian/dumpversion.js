
var util = require('util');
var http = require('http');
var url = require('url');
var npm = require('npm');
var npm2debian = require('./lib/cli.js');
var path = require('path');
var Q = require('./q'); // want new Q.allResolved...
var fs = require('fs');

var opts = {
	versioned: false,
	maintainer: 'Ricky Ng-Adam',
	email: 'rngadam@lophilo.com',
	noPackagePrefix: false,
	packagePrefix: 'lpl-npm-',
	debBuild: '1',
	output: 'out'
};

var tree = {
	maxDepth: 0,
  pending: 1 
};

function getPackage(name, cb) {
	var options = {
	  host: 'registry.npmjs.org',
	  port: 80,
	  path: '/' + name
	};
	http.get(options, function(res) {
	  var data = '';
	  res.on('data', function(chunk) {
		  data += chunk;
	  });
	  res.on('end', function(err) {
		  cb(JSON.parse(data))
	  });
	}).on('error', function(e) {
	  console.log("Got error: " + e.message);
	});
}

exports.getPackage = getPackage;

function clone(obj) {
  return JSON.parse(JSON.stringify(obj));
}


function checkParams(params) {
  if(!params.tree.maxDepth) {
    console.log('invalid maxDepth property');
    process.exit(1);
  }
  if(!params || !params.tree || !params.dependencies) {
    console.log('invalid input to writeMakefile');
    process.exit(1);
  }
}

function debianizeTree(params) {
  console.log(arguments.callee.name); console.log(arguments);
  errors = [];
  promises = [];
  checkParams(params);
  for(var currentDepth = params.tree.maxDepth; 
      currentDepth>0; 
      currentDepth--) {
    console.log('processing depth ' + currentDepth);
    names = clone(params.tree[currentDepth]);
    while(name = names.pop()) {
      if(name === 'serve')
        continue;
      console.log('scheduling debianizing of ' + name);
      opts.dependencies = params.dependencies[name].deps;
      opts.devDependencies = params.dependencies[name].devdeps;
      // debianize returns a promise to debianize eventually...
      promises.push(debianize(name, opts).fail(
              function(err) {
                console.log(
                  'npm2debian.debianize failed: ' + err);
                errors.push(err); 
                process.exit(1);
              }
            )
          );
    }
  }
  return Q.allResolved(promises).then(function() {
    console.log('debianization complete');
    return params;
  });
}

function writePkgNames(params) {
  console.log(arguments.callee.name); console.log(arguments);
  return Q.ncall(
    fs.writeFile, 
    fs, 
    opts.output + '/pkgnames.json', 
    JSON.stringify(params.pkgNames)).then(
    function() {return params});
}

function readPkgNames(params) {
  console.log(arguments.callee.name); console.log(arguments);
  return Q.ncall(
    fs.readFile, 
    fs, 
    opts.output + '/pkgnames.json', 
    'utf-8').then(function(output) {
      params.pkgNames = JSON.parse(output);
      return params;
  });
}

dependencies = {};
dependedOn = {};

function readTreeDependencies(target, treeFn, dependenciesFn) {
  console.log(arguments.callee.name); console.log(arguments);
  return Q.all([
      Q.ncall(fs.readFile, fs, treeFn, "utf-8")
        .then(JSON.parse),
      Q.ncall(fs.readFile, fs, dependenciesFn, "utf-8")
        .then(JSON.parse)
  ]).then(function(params) {
      return {'tree': params[0], 'dependencies': params[1]};
    });
}

function getTree(desc, depth, cb) {
  if(!desc) {
    console.log('invalid descriptor, aborting');
    process.exit(1);
  }
	//console.log(desc);
	if(!tree[depth]) {
		tree[depth] = [];
		if(tree.maxDepth < depth)
			tree.maxDepth = depth;
	}
	tree[depth].push(desc.name);

	var ver = desc['dist-tags'].latest;
  dependencies[desc.name] = { 
    deps: desc.versions[ver].dependencies, 
    devdeps: desc.versions[ver].devDependencies, 
    alldeps: {} 
  }; 
  for(var k in desc.versions[ver].devDependencies) {
    dependencies[desc.name].alldeps[k] = desc.versions[ver].devDependencies[k];
  }
  for(var k in desc.versions[ver].dependencies) {
    dependencies[desc.name].alldeps[k] = desc.versions[ver].dependencies[k];
  }

  fetchQueue = [];
  // check see if these deps were already seen
  for(var dep in dependencies[desc.name].alldeps) {
    if(dependedOn[dep]) {
      dependedOn[dep].push(desc.name);
      // we've already scheduled fetching it elsewhere!
    } else {
      dependedOn[dep] = [desc.name];
      fetchQueue.push(dep);
    }
  }

	for(var i=0; i<fetchQueue.length; i++) {
    //console.log('fetching ' + fetchQueue[i]);
    tree.pending++;
		console.log(Array(depth).join('>') + fetchQueue[i]);
		getPackage(fetchQueue[i], function(desc) {
      getTree(desc, depth+1, cb);
    }); 
	}
  tree.pending--;
	if(tree.pending == 0) {
    cb({'tree': tree, 'dependencies': dependencies});
	}
}

function writeMakefile(params) {
  console.log(arguments.callee.name); console.log(arguments);
  checkParams(params);
  //console.log(arguments.callee.name); console.log(arguments);
  output = '';
  suffix = '.deb';
  if(currentDepth == 0) {
    name = params.tree['0'][0];
    pkgname = params.pkgNames[name].archive;
    output += '.PHONY: all';
    output += 'all: ' + pkgname + suffix + '\n';
  }
  uninstall = [];
  for(var currentDepth = params.tree.maxDepth; 
      currentDepth >= 0; 
      currentDepth--) {
    if(!params.tree[currentDepth]) {
      console.log('Error, invalid depth ' + currentDepth);
      console.log(util.inspect(params.tree));
      process.exit(1);
    }
    names = clone(params.tree[currentDepth]);
    while(name = names.pop()) {
      if(name === 'serve')
        continue;
      console.log('creating Makefile target ' + name);
      pkgname = params.pkgNames[name].archive;
      pkgdir = params.pkgNames[name].dir;
      alldeps = [];
      for(k in params.dependencies[name].alldeps) {
        if(k === 'serve')
          continue;
        console.log('pushing ' + k);
        alldeps.push(params.pkgNames[k].archive);
      }
      if(!pkgname || !pkgdir || !alldeps) {
        console.log('Invalid dependency information for ' + name + ':');
        console.log(util.inspect(params.dependencies[name]));
        process.exit(1);
      }
      output += pkgname + suffix + ': ' + (alldeps.length>0?alldeps.join(suffix + ' ') + suffix + '\n': '\n');
      output += '\t@echo Building package ' + name + '\n';
      output += '\tcd ./' + pkgdir + '; dpkg-buildpackage -b -k37FC6E55 -d -rfakeroot\n';
      output += '\tsudo dpkg -i ${@}' + '\n\n';
      uninstall.push(pkgname);
    }
  }

  output += 'uninstall:\n'
  while(pkgname = uninstall.pop()) { 
    output += '\t-sudo dpkg -r ' + pkgname + '\n'; 
  }
  console.log('Makefile output: ' + output);
  return Q.ncall(
      fs.writeFile, fs, opts.output + '/Makefile', output).then(function() {
    console.log('Makefile written out!');
  });
}

function getTreeDependencies(target) {
  console.log(arguments.callee.name); console.log(arguments);
  var deferred = Q.defer();
  console.log('starting with ' + target);
  getPackage(target, function(desc) {
    getTree(desc, 0, function(params) {
      deferred.resolve(params);
    });
  });
  return deferred.promise;
}

function writeTreeDependencies(params, treeFn, depsFn) {
  console.log(arguments.callee.name); console.log(arguments);
  return Q.all([
    Q.ncall(fs.writeFile, fs, treeFn, JSON.stringify(params.tree)),
    Q.ncall(fs.writeFile, fs, depsFn, JSON.stringify(params.dependencies))
    ]).then(function() { return params});
}

PARAMS = {};
function passParams(params) {
  for(var k in params) {
    PARAMS[k] = params[k];
  }
  return PARAMS;
}

if(opts.output === "test") {
  getNameToPackageName = function getNameToPackageName(name) {
    return name + '-rev';
  }
  debianize = function(name, opts) {
    return Q.call(console.log, console, 'fake debianization ' + name);
  }
} else {
  getNameToPackageName = npm2debian.getNameToPackageName;
  debianize = npm2debian.debianize;
}


function updatePkgNames(params) {
  console.log(arguments.callee.name); console.log(arguments);
  params.pkgNames = {};
  console.log(util.inspect(params.dependencies));
  for(var k in params.dependencies) {
    params.pkgNames[k] = getNameToPackageName(k);
  }
  return params;
}

function execute(target, treeFn, depsFn) {
  console.log(arguments.callee.name); console.log(arguments);
  return Q
    .call(readTreeDependencies, 
        null, target, treeFn, depsFn)
    .then(
        passParams, 
        function(err) { 
          console.log(err);
          console.log('Falling back to server fetch');
          return getTreeDependencies(target).then(
            function(params) {
              return writeTreeDependencies(params, treeFn, depsFn)
            }
            ) 
        }
      )
    .then(readPkgNames)
    .then(
        passParams, 
        function(err) {
          console.log('Reading package failed: ' + err);
          return Q.call(debianizeTree, null, PARAMS)
            .then(updatePkgNames)
            .then(writePkgNames)
        })
    .then(writeMakefile)
    .then(function() {
        console.log('success!!');
      })
    .fail(function(err) { 
      console.log('failure handler: ' + err); 
      process.exit(1);
      }
    );
}

main = function(target) {
  console.log(arguments.callee.name); console.log(arguments);
  conf = {
    'cache': path.resolve('.cache')
  };

  return Q
    .ncall(npm.load, npm, conf)
    .then(
      function() {
        target = 'socket.io';
        treeFn = opts.output + '/tree.json'; 
        depsFn = opts.output + '/dependencies.json'; 
        return [target, treeFn, depsFn];
      })
    .spread(execute)
    .fail(function(err) {
      console.log('could not execute ' + err);
      }
     )
}
exports.execute = execute;

if (module === require.main) {
  main('socket.io');
  http.createServer(function (req, res) {}).listen(process.env.PORT || 8080, "0.0.0.0");
}
