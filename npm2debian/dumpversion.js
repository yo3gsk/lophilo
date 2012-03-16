
var util = require('util');
var http = require('http');
var url = require('url');
var npm = require('npm');
var npm2debian = require('./lib/cli.js');
var path = require('path');
var Q = require('qq');

function getPackage(name, cb) 
{
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

function debianizeSublevel(tree, currentDepth)
{
  p = [];
  while(name = tree[currentDepth].pop()) {
    console.log('depth ' + currentDepth + ' debianizing ' + name);
    p.push(npm2debian.debianize(name, opts));
  }
  if(currentDepth > 0) {
    return Q.all(p).then(debianizeSublevel(tree, currentDepth-1));
  } else {
    return Q.all(p);
  }
}

function debianizeTree(tree) 
{
  return debianizeSublevel(tree, tree.max);
}

function getTree(desc, depth, cb) 
{
  console.log('pending: ' + tree.pending);
	//console.log(desc);
	if(!tree[depth]) {
		tree[depth] = [];
		if(tree.max < depth)
			tree.max = depth;
	}
	tree[depth].push(desc.name);

	var ver = desc['dist-tags'].latest;
	dependencies = desc.versions[ver].dependencies;
	for(var key in dependencies) {
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

exports.process = function(target) 
{
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
          deferred.resolve(true);
        }, function(error) { 
          console.log(error);
        });
      });
    });
  });
  return deferred.promise;
}

exports.process('socket.io');

http.createServer(function (req, res) {}).listen(process.env.PORT || 8080, "0.0.0.0");
