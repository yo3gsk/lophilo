#!/usr/bin/env node
var util = require('util');
var http = require('http');
var url = require('url');
var npm = require('npm');
var npm2debian = require('./lib/cli.js');
var path = require('path');
var Q = require('./q'); // want new Q.allResolved...
var fs = require('fs');

function getPackagePromise(name) {
  var deferred = Q.defer();
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
		  deferred.resolve(JSON.parse(data));
	  });
	}).on('error', function(e) {
	  console.log("Got error: " + e.message);
    deferred.reject(e);
	});
  return deferred.promise;
}

function clone(obj) {
  return JSON.parse(JSON.stringify(obj));
}


function checkParams(params) {
  if(!params.tree.hasOwnProperty('maxDepth')) {
    console.log('invalid maxDepth property');
    throw new Error("Error occurred");
  }
  if(!params || !params.tree || !params.dependencies) {
    console.log('invalid input to writeMakefile');
    throw new Error("Error occurred");
  }
}

var debianizeParams = [
  'maintainer', 
  'email', 
  "versioned",
  "noPackagePrefix",
  "packagePrefix",
  "debBuild"
];

function prepareDebianizeParams(params, name) {
  ret = {};
  ret.dependencies = params.dependencies[name].deps;
  ret.devDependencies = params.dependencies[name].devdeps;
  ret.output = params.workdir;
  console.log(debianizeParams);
  console.log(debianizeParams.length);
  for(var i=0; i<debianizeParams.length; i++) {
    name = debianizeParams[i];
    ret[name] = params[name];
    if(!ret.hasOwnProperty(name)) {
      throw new Error('parameter not found when expected ' + name);
    }
  }
  console.log('using ' + util.inspect(ret));
  return ret;
}

function debianizeTree(params) {
  logParams(arguments);
  errors = [];
  promises = [];
  checkParams(params);
  for(var currentDepth = params.tree.maxDepth; 
      currentDepth>=0; 
      currentDepth--) {
    console.log('processing depth ' + currentDepth);

    if(!params.tree.hasOwnProperty(currentDepth)) {
      console.log('Invalid tree object, no such depth ' + currentDepth);
      throw new Error("Error occurred");
    }
    names = clone(params.tree[currentDepth]);
    while(name = names.pop()) {
      if(name === 'serve')
        continue;
      console.log('scheduling debianizing of ' + name);
      dParams = prepareDebianizeParams(params, name);
      // debianize returns a promise to debianize eventually...
      promises.push(params.fnc.debianize(name, dParams).fail(
              function(err) {
                console.log(
                  'npm2debian.debianize failed: ' + err);
                errors.push(err); 
                throw new Error("Error occurred");
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
  logParams(arguments);
  return Q.ncall(
    fs.writeFile, 
    fs, 
    params.workdir + '/pkgnames.json', 
    JSON.stringify(params.pkgNames)).then(
    function() {return params});
}

function readPkgNames(params) {
  logParams(arguments);
  return Q.ncall(
    fs.readFile, 
    fs, 
    params.workdir + '/pkgnames.json', 
    'utf-8').then(function(output) {
      params.pkgNames = JSON.parse(output);
      return params;
  });
}

function readTreeDependencies(params) {
  logParams(arguments);
  return Q.all([
      Q.ncall(fs.readFile, fs, params.treeFn, "utf-8")
        .then(JSON.parse),
      Q.ncall(fs.readFile, fs, params.depsFn, "utf-8")
        .then(JSON.parse)
  ]).then(function(results) {
      params.tree = results[0];
      params.dependencies = results[1];
      return params;
    });
}

function validateDesc(desc) {
  if(!desc || !desc["dist-tags"] || !desc.versions) {
    console.log(util.inspect(desc));
    console.log('invalid description');
    throw new Error("Error occurred");
  }
}

function updateTree(params, desc, depth) {
	if(!params.tree[depth]) {
		params.tree[depth] = [];
		if(params.tree.maxDepth < depth)
			params.tree.maxDepth = depth;
	}
	params.tree[depth].push(desc.name);

	var ver = desc['dist-tags'].latest;
  if(!ver) {
    console.log("Invalid version!");
    throw new Error("Error occurred");
  }
  console.log('latest version of ' + desc.name + ' is ' + ver);
  params.dependencies[desc.name] = { 
    "deps": desc.versions[ver].dependencies, 
    "devdeps": desc.versions[ver].devDependencies, 
    "alldeps": {} 
  }; 
  for(var k in desc.versions[ver].devDependencies) {
    params.dependencies[desc.name].alldeps[k] = desc.versions[ver].devDependencies[k];
  }
  for(var k in desc.versions[ver].dependencies) {
    params.dependencies[desc.name].alldeps[k] = desc.versions[ver].dependencies[k];
  }
}
function processDesc(params, desc, depth) {
    updateTree(params, desc, depth);
    queue = [];
    for(var dep in params.dependencies[desc.name].alldeps) {
      currentDepth = depth+1;
      if(params.alreadyQueued[dep]) {
        if(params.alreadyQueued[dep] < currentDepth) {
          params.alreadyQueued[dep] = currentDepth;
        }
        continue;
      } else {
        params.alreadyQueued[dep] = currentDepth;
      }
      newPromise = params.fnc.getPackage(dep, currentDepth)
        .then(function(desc) {
          return processDesc(params, desc, currentDepth);
        });
      queue.push(newPromise);
    }
    return Q.all(queue);
}

function getTreeDependenciesPromise(params) {
  params.tree = {
    maxDepth: 0,
  };
  params.dependencies = {};
  params.dependedOn = {};
  params.alreadyQueued = {};
  return params.fnc.getPackage(params.target)
    .then(function(desc) {
      return processDesc(params, desc, 0);
    })
    .then(function() {
      return params;
    });
}

function logParams(args) {
  //console.log(args.callee.name); console.log(args);
}

function writeMakefile(params) {
  logParams(arguments);
  checkParams(params);
  //logParams(arguments);
  output = '';
  suffix = '.deb';
  // all target
  name = params.tree['0'][0];
  pkgname = params.pkgNames[name].archive;
  output += '.PHONY: all uninstall\n\n';
  output += 'all: ' + pkgname + suffix + '\n\n';

  uninstall = [];
  for(var currentDepth = params.tree.maxDepth; 
      currentDepth >= 0; 
      currentDepth--) {
    if(!params.tree.hasOwnProperty(currentDepth)) {
      console.log('Invalid tree object, no such depth ' + currentDepth);
      throw new Error("Error occurred");
    }
    names = clone(params.tree[currentDepth]);
    while(name = names.pop()) {
      if(name === 'serve')
        continue;
      console.log('creating Makefile target ' + name);
      pkglogical = params.pkgNames[name].logical;
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
        throw new Error("Error occurred");
      }
      output += pkgname + suffix + ': ' + (alldeps.length>0?alldeps.join(suffix + ' ') + suffix + '\n': '\n');
      output += '\t@echo Building package ' + name + '\n';
      output += '\tcd ./' + pkgdir + '; dpkg-buildpackage -b -k37FC6E55 -d -rfakeroot\n';
      output += '\tsudo dpkg -i ${@}' + '\n\n';
      uninstall.push(pkglogical);
    }
  }

  output += 'uninstall:\n'
  while(pkgname = uninstall.pop()) { 
    output += '\t-sudo dpkg -r ' + pkgname + '\n'; 
  }
  console.log('Makefile output:\n' + output);
  return Q.ncall(
      fs.writeFile, fs, params.workdir + '/Makefile', output).then(function() {
    console.log('Makefile written out!');
  });
}

function writeTreeDependencies(params) {
  logParams(arguments);
  return Q.all([
    Q.ncall(fs.writeFile, fs, params.treeFn, JSON.stringify(params.tree)),
    Q.ncall(fs.writeFile, fs, params.depsFn, JSON.stringify(params.dependencies))
    ]).then(function() { 
      return params; 
    });
}

function updatePkgNames(params) {
  logParams(arguments);
  params.pkgNames = {};
  console.log(util.inspect(params.dependencies));
  for(var k in params.dependencies) {
    params.pkgNames[k] = params.fnc.getNameToPackageName(k);
    if(!params.pkgNames[k]) {
      throw new Error('Could not find package name for ' + k);
    }
  }
  return params;
}

function passParams(params) {
  return params;
}
function execute(params) {
  logParams(arguments);
  return Q
    .call(readTreeDependencies, 
        null, params)
    .then(
        passParams, 
        function(err) { 
          console.log(err);
          console.log('Falling back to server fetch');
          return getTreeDependenciesPromise(params).then(
            function(params) {
              return writeTreeDependencies(params);
            }); 
        }
      )
    .then(readPkgNames)
    .then(
        passParams, 
        function(err) {
          console.log('Reading package failed: ' + err.stack);
          return Q.call(debianizeTree, null, params)
            .then(updatePkgNames)
            .then(writePkgNames)
        })
    .then(writeMakefile)
    .then(function() {
        console.log('success!!');
      })
    .fail(function(err) { 
      console.log('top failure handler: '); 
      console.log(err.stack);
      throw new Error("Error occurred");
      }
    );
}

testConfig = {
  "versioned": false,
  "maintainer": "Ricky Ng-Adam",
  "email": "rngadam@lophilo.com",
  "noPackagePrefix": false,
  "packagePrefix": "lpl-npm-",
  "debBuild": "1",
  "target": "root"
};

function setupDummyFunctions(params) {

  params.fnc.getNameToPackageName = function getNameToPackageName(name) {
    return {
      'logical': name + '-rev',
      'dir': name + '-dir',
      'archive': name + '-rev' };
  }

  params.fnc.debianize = function(name, params) {
    return Q.call(console.log, console, 'fake debianization ' + name);
  }

  params.fnc.getPackage = function(name) {
    console.log("getPackage"); console.log(arguments);
    return Q.call(function() {
      desc = {};
      desc.name = name;
      version = name + '-version';
      desc["dist-tags"] = { latest: version };
      desc.versions = {};
      desc.versions[version] = {};
      if(name === "root") {
        desc.versions[version].dependencies = {"directdep": 1};
        desc.versions[version].devDependencies = {"devdep": 1};
      }
      if(name === "directdep") {
        desc.versions[version].dependencies = {"leaf": 1};
        desc.versions[version].devDependencies = {};
      }
      if(name === "leaf") {
        desc.versions[version].dependencies = {};
        desc.versions[version].devDependencies = {};
      }
      if(name === "devdep") {
        desc.versions[version].dependencies = {};
        desc.versions[version].devDependencies = {};
      }
      return desc;
    });
  }
}

function setupTestDir(workdir) {
  return Q
    .ncall(require('rimraf'), null, workdir)
    .then(function() {
        fs.mkdirSync(workdir);
        fs.writeFileSync(
          workdir + '/config.json', 
          JSON.stringify(testConfig));
  });
}
function setupTest(params) {
    return Q.call(function() {
      setupDummyFunctions(params); })
    .fail(function(err) {
      console.log(err);
      console.log('test setup failed!');
      throw new Error("Error occurred"); });
  
}
main = function() {
  logParams(arguments);
  conf = {
    'cache': path.resolve('.cache')
  };

  require('coa').Cmd()
      .name(path.basename(process.argv[1]))
      .helpful()
      .arg()
          .name('workdir').title('Work directory (must contain a config.js file)')
          .req()
          .end()
      .completable()
      .act(function(opts, args) {
          console.log(util.inspect(opts));
          console.log(util.inspect(args));
          return Q
            .call(npm.load, npm, conf)
            .then(function() {
                if(args.workdir === "test") {
                  return setupTestDir(args.workdir);
                }
            })
            .then(
              function() {
                return Q.ncall(
                  fs.readFile, 
                  fs, 
                  path.join(args.workdir, 'config.json'));
              })
            .then(JSON.parse)
            .then(
              function(params) {
                params.workdir = args.workdir;
                params.treeFn = params.workdir + '/tree.json'; 
                params.depsFn = params.workdir + '/dependencies.json'; 
                return params;
              })
            .then(
              // dependency injection
              function(params) {
                params.fnc = {};
                if(params.workdir === "test") {
                  setupTest(params);
                } else {
                  params.fnc.getPackage = getPackagePromise;
                  params.fnc.getNameToPackageName = npm2debian.getNameToPackageName;
                  params.fnc.debianize = npm2debian.debianize;
                }
                return params;
              })
            .then(execute);
        }).run();
}
exports.execute = execute;

if (module === require.main) {
  main();
}
