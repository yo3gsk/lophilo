
var util = require('util');
var http = require('http');
var url = require('url');
var npm = require('npm');
var npm2debian = require('./lib/cli.js');
var path = require('path');
var Q = require('qq');

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
	max: 0,
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

/*
 * Makefile
 *
 * <package name>.deb: <other deb this depends on>
 *    dpkg-buildpackage -b -k37FC6E55 -d -rfakeroot
 *    dpkg -i <package_name>.deb
 */
function createMakefile(output, tree, dependencies, currentDepth) {
  while(name = tree[currentDepth].pop()) {
    pkgdir = dependencies[name].pkgdir;
    alldeps = dependencies[name].alldeps;
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

function debianizeSublevel(tree, currentDepth, dependencies, debianize, lookupPkgDir) {
  p = [];
  while(name = tree[currentDepth].pop()) {
    console.log('depth ' + currentDepth + ' debianizing ' + name);
    opts.dependencies = dependencies[name].deps;
    opts.devDependencies = dependencies[name].devdeps;
    p.push(debianize(name, opts)).then(function() {
      dependencies[name].pkgdir = (name);
      return dependencies[name].pkgdir;
    });
  }
  if(currentDepth > 0) {
    return Q.all(p).then(debianizeSublevel(tree, currentDepth-1, dependencies, debianize, lookupPkgDir));
  } else {
    return Q.all(p);
  }
} 
exports.debianizeSublevel = debianizeSublevel;

function debianizeTree(tree) {
  return debianizeSublevel(tree, tree.max, dependencies, npm2debian.debianize, npm2debian.getNameToPackageDir);
}

dependencies = {};
dependedOn = {};

function getTree(desc, depth, cb) {
  console.log('pending: ' + tree.pending);
	//console.log(desc);
	if(!tree[depth]) {
		tree[depth] = [];
		if(tree.max < depth)
			tree.max = depth;
	}
	tree[depth].push(desc.name);

	var ver = desc['dist-tags'].latest;
  alldeps = desc.versions[ver].dependencies;
  for(var k in desc.versions[ver].devDependencies) {
    alldeps[k] = desc.versions[ver].devDependencies[k];
  }
  dependencies[desc.name] = { 
    deps: desc.versions[ver].dependencies, 
    devdeps: desc.versions[ver].devDependencies, 
    alldeps: alldeps 
  }; 

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
    cb(tree);
	}
}

exports.process = function(target) {
  var deferred = Q.defer();
  conf = {
    'cache': path.resolve('.cache')
  };

  npm.load(conf, function(err) {
    if(err) {
      console.log('error initializing npm');
      return;
    }
    console.log(target);
    getPackage(target, function(desc) {
      getTree(desc, 0, function(tree) {
        debianizeTree(tree).then(function() {
          makefile = createMakefile('', tree, dependencies, tree.maxdepth);
          console.log(makefile);
          fs.writeFile(opts.output + '/Makefile', makefile, function(err) {
              if(err) {
                console.log(err);
              } else {
                console.log("The file was saved!");
              }
          }); 
          deferred.resolve(true);
        }, function(error) { 
          console.log(error);
        });
      });
    });
  });
  return deferred.promise;
}


if (module === require.main) {
  exports.process('socket.io');
    http.createServer(function (req, res) {}).listen(process.env.PORT || 8080, "0.0.0.0");
}
