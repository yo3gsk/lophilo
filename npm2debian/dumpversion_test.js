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

console.log(dumpversion.createMakefile('', tree, dependencies, tree.maxdepth));

promise = dumpversion.debianizeSublevel(
    tree, 
    tree.maxdepth, 
    dependencies, 
    function(a, o) {
      console.log('debianizing ' + a); 
    }, 
    function(name) {
      console.log('looking up name ' + name); 
      return dependencies[name]['pkgdir'];
    }
    );
promise.wait();
