
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
	packagePrefix: 'lophilo-npm-',
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

function clone(obj) {
  return JSON.parse(JSON.stringify(obj));
}

function fakeDebianize(name, opts) {
  return Q.call(console.log, console, 'fake debianization ' + name);
}

function debianizeTree(params) {
  console.log(arguments.callee.name); console.log(arguments);
  errors = [];
  promises = [];
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
      promises.push(npm2debian.debianize(name, opts).fail(
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

  // check see if these deps were already seen
  for(var d in dependencies[desc.name].alldeps) {
    if(dependedOn[d]) {
      dependedOn[d].push(desc.name);
      // we've already scheduled fetching it...
      delete dependencies[desc.name].alldeps[d];
    } else {
      dependedOn[d] = [desc.name];
    }
  }

	for(var key in dependencies[desc.name].alldeps) {
    tree.pending++;
		console.log(Array(depth).join('>') + key);
		getPackage(key, function(desc) {
      getTree(desc, depth+1, cb);
    }); 
	}
  tree.pending--;
	if(tree.pending == 0) {
    cb({'tree': tree, 'dependencies': dependencies});
	}
}

function writeMakefile(params) {
  if(!params || !params.tree || !params.dependencies) {
    console.log('invalid input to writeMakefile');
    process.exit(1);
  }
  //console.log(arguments.callee.name); console.log(arguments);
  output = '';
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
      pkgdir = npm2debian.getNameToPackageName(name);
      alldeps = params.dependencies[name].alldeps;
      if(!pkgdir || !alldeps) {
        console.log('Invalid dependency information for ' + name + ':');
        console.log(util.inspect(params.dependencies[name]));
        process.exit(1);
      }
      output += pkgdir + '.deb: ' + (Object.keys(alldeps).length>0?Object.keys(alldeps).join('.deb, ') + '.deb\n': '\n');
      output += '\tdpkg-buildpackage -b -k37FC6E55 -d -rfakeroot ' + pkgdir + '\n';
      output += '\tdpkg -i ${@}' + '\n';
    }
  }
  console.log('my output: ' + output);
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

function execute(target, treeFn, depsFn) {
  console.log(arguments.callee.name); console.log(arguments);
  return Q
    .call(readTreeDependencies, 
        null, target, treeFn, depsFn)
    .then(
        function(params) {return params; }, 
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
    .then(debianizeTree)
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
