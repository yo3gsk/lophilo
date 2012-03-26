#!/usr/bin/env node
var util = require('util');
var http = require('http');
var url = require('url');
var path = require('path');
var fs = require('fs');
var n2d = require('./lib');

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

function updateTree(injections, desc, version) {
  if(version == null) {
    var ver = desc['dist-tags'].latest;
  }
  if(!ver) {
    console.log("Invalid version!");
    throw new Error("Error occurred");
  }
  console.log('selected version of ' + desc.name + ' is ' + ver);
  deps = [];
  for(var k in desc.versions[ver].devDependencies) {
    devdep = desc.versions[ver].devDependencies[k];
    injections.datastore.addDevDep(desc.name, version, devdep);
  }
  for(var k in desc.versions[ver].dependencies) {
    injections.datastore.addDep(injections.target, desc.versions[ver].dependencies[k]);
  }
}

function processDesc(injections, desc, version) {
    queue = [];
    deps = updateTree(injections, desc.name, version);
    for(var dep in deps) {
      if(injections.alreadyQueued[dep]) {
        continue;
      } else {
        injections.alreadyQueued[dep] = true;
      }
      newPromise = injections.fnc.getPackage(dep)
        .then(function(desc) {
          return processDesc(injections, desc);
        });
      queue.push(newPromise);
    }
    return Q.all(queue);
}

function getTreeDependenciesPromise(injections, target) {
  injections.alreadyQueued = {};
  return injections.fnc.getPackage(injections.target)
    .then(function(desc) {
      return processDesc(injections, desc, null);
    })
    .then(function() {
      return injections;
    });
}


function execute(injections, target) {
  n2d.loginjections(arguments);
  return getTreeDependenciesPromise(injections, target);
    .fail(function(err) { 
      console.log('top failure handler: '); 
      console.log(err.stack);
      throw new Error("Error occurred");
      }
    );
}
exports.execute = execute;

main = function() {
  require('coa').Cmd()
      .name(path.basename(process.argv[1]))
      .helpful()
      .arg()
          .name('workdir').title('Work directory (must contain a config.json file)')
          .req()
          .end()
      .arg()
          .name('target').title('target package (must contain a config.json file)')
          .req()
          .end()
      .completable()
      .act(function(opts, args) {
          console.log(util.inspect(opts));
          console.log(util.inspect(args));
          injections = {};
          injections.datastore = datastore(args.workdir);
          return execute(injections, args.target);
      }).run();
}

if (module === require.main) {
  main();
}
