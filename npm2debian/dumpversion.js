
var util = require('util');
var http = require('http');
var url = require('url');
var npm = require('npm');
var npm2debian = require('./lib/cli.js');
var path = require('path');
var Q = require('qq');
var fs = require('fs');

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
	maxdepth: 0,
  pending: 1 
};

/*
function linkDependencies(dep, dir) {
  var deferred = Q.defer();
  npm.commands.link([dep, dir], function(e) { 
    console.log(e);
    deferred.resolve(true);
  });
  return deferred.promise;
}

function linkAllDependencies(name) {
    depsLinkPromises = [];
    for(var dep in dependencies[name].alldeps) {
      depsLinkPromises.push(linkDependency(name, npm2debian.getNameToPackageDir(name)));
    }
    return Q.all(depsLinkPromises);
}
*/

function clone(obj) {
  return JSON.parse(JSON.stringify(obj));
}
/*
 * Makefile
 *
 * <package name>.deb: <other deb this depends on>
 *    dpkg-buildpackage -b -k37FC6E55 -d -rfakeroot
 *    dpkg -i <package_name>.deb
 */
function createMakefile(output, tree, dependencies, currentDepth) {
  if(!tree[currentDepth]) {
    console.log('Error, invalid depth ' + currentDepth);
    console.log(util.inspect(tree));
    process.exit(1);
  }
  names = clone(tree[currentDepth]);
  while(name = names.pop()) {
    pkgdir = npm2debian.getNameToPackageDir(name);
    alldeps = dependencies[name].alldeps;
    if(!pkgdir || !alldeps) {
      console.log('Invalid dependency information for ' + name + ':');
      console.log(util.inspect(dependencies[name]));
      process.exit(1);
    }
    output += pkgdir + '.deb: ' + (Object.keys(alldeps).length>0?Object.keys(alldeps).join('.deb, ') + '.deb\n': '\n');
    output += '\tdpkg-buildpackage -b -k37FC6E55 -d -rfakeroot ' + pkgdir + '\n';
    output += '\tdpkg -i ${@}' + '\n';
  }
  if(currentDepth > 0) {
    return createMakefile(output, tree, dependencies, currentDepth-1);
  } else {
    return output;
  }
}

exports.createMakefile = createMakefile;

function debianizeTree(inputs) {
  console.log(arguments.callee.name); console.log(arguments);
  var deferred = Q.defer();
  n = deferred.promise;
  for(var currentDepth = inputs.tree.maxdepth; 
      currentDepth>0; 
      currentDepth--) {
    console.log('processing depth ' + currentDepth);
    names = clone(inputs.tree[currentDepth]);
    promises = [];
    while(name = names.pop()) {
      console.log(' debianizing ' + name);
      opts.dependencies = inputs.dependencies[name].deps;
      opts.devDependencies = inputs.dependencies[name].devdeps;
      promises.push(npm2debian.debianize(name, opts).fail(function(err) {console.log(err); process.exit(1);}));
    }
    n = n.then(Q.all(promises));
  }
  n = n.then(function() {return inputs;});
  return n;
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
  ]).then(function(inputs) {
      return {'tree': inputs[0], 'dependencies': inputs[1]};
    });
}

function getTree(desc, depth, cb) {
	//console.log(desc);
	if(!tree[depth]) {
		tree[depth] = [];
		if(tree.maxdepth < depth)
			tree.maxdepth = depth;
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

function writeMakefile(inputs) {
  console.log(arguments.callee.name); console.log(arguments);
  makefile = createMakefile(
      '', inputs.tree, inputs.dependencies, inputs.tree.maxdepth);
  return Q.ncall(
      fs.writeFile, fs, opts.output + '/Makefile', makefile);
}

function getTreeDependencies(target) {
  console.log(arguments.callee.name); console.log(arguments);
  var deferred = Q.defer();
  console.log('starting with ' + target);
  getPackage(target, function(desc) {
    getTree(desc, 0, function(inputs) {
      deferred.resolve(inputs);
    });
  });
  return deferred.promise;
}

function writeTreeDependencies(inputs, treeFn, depsFn) {
  console.log(arguments.callee.name); console.log(arguments);
  return Q.all([
    Q.ncall(fs.writeFile, fs, treeFn, JSON.stringify(inputs.tree)),
    Q.ncall(fs.writeFile, fs, depsFn, JSON.stringify(inputs.dependencies))
    ]).then(function() { return inputs});
}

function convertToDebian(inputs) {
  console.log(arguments.callee.name); console.log(arguments);
  return ;
}

function execute(target, treeFn, depsFn) {
  console.log(arguments.callee.name); console.log(arguments);
  return Q
    .call(readTreeDependencies, 
        null, target, treeFn, depsFn)
    .then(
        function(inputs) {return inputs; }, 
        function(err) { 
          console.log(err);
          console.log('Falling back to server fetch');
          return getTreeDependencies(target).then(
            function(inputs) {
              return writeTreeDependencies(inputs, treeFn, depsFn)
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
