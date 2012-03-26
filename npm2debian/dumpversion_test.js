var Q = require('qq');
dumpversion = require('./dumpversion.js');

tree = {
  maxdepth: 2,
  '2': ['leaf'],
  '1': ['direct', 'devdirect'],
  '0': ['root'],
};

dependencies = {
  root: {
    pkgdir: 'rootdir',
    alldeps: {'devdirect': true, 'direct': true},
    devdeps: {'devdirect': true},
    deps: {'direct': true},
  },
  direct: {
    pkgdir: 'directdir',
    alldeps: {'leaf': true},
    devdeps: {},
    deps: {'leaf': true},
  },
  devdirect: {
    pkgdir: 'devdirectdir',
    alldeps: {},
    devdeps: {},
    deps: {},
  },
  leaf: {
    pkgdir: 'leafdir',
    alldeps: {},
    devdeps: {},
    deps: {},
  },
};

function test_createMakefile() {
  deferred = Q.defer();
  setTimeout(function() { deferred.resolve(true); }, 500);
  console.log(dumpversion.createMakefile(
      '', 
      tree, 
      dependencies, 
      tree.maxdepth));
  return deferred.promise;
}

function test_debianizeSubLevel() {
    return dumpversion.debianizeSublevel(
        tree, 
        tree.maxdepth, 
        dependencies, 
        function(a, o) {
          deferred = Q.defer();
          setTimeout(function() { deferred.resolve(true); }, 500);
          console.log('debianizing ' + a); 
          return deferred.promise;
        }, 
        function(name) {
          console.log('looking up name ' + name); 
          return dependencies[name]['pkgdir'];
        });
}
require('coa').Cmd()
  .act(function(opts, args) {
    return Q.step(
      test_createMakefile,  
      test_debianizeSubLevel
      )
  })
  .run();
