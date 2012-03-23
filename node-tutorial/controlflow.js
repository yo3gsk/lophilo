Q = require('qq');

testdata = {
  'tree': 'treevalue', 
  'dependencies': 'dependenciesvalue'
}

//promises tree, dependencies
function readTreeDependencies(target, treeFilename, dependenciesFilename) {
  console.log(arguments.callee.name); console.log(arguments);
  //return testdata; 
  throw new Error("Can't read data");
}

// promises tree, dependencies
function getTreeDependencies(target) {
  console.log(arguments.callee.name); console.log(arguments);
  return testdata;
}

// promises disk-stored tree
function writeTree(tree) {
  console.log(arguments.callee.name); console.log(arguments);
  return true;
}

// promises disk-stored dependencies
function writeDependencies(dependencies) {
  console.log(arguments.callee.name); console.log(arguments);
  return true;
}

//promises to create Makefile
function writeMakefile(tree, dependencies) {
  console.log(arguments.callee.name); console.log(arguments);
  return [tree, dependencies];
}

// promises to fetch and debianize packages
function debianize(tree, dependencies) {
  console.log(arguments.callee.name); console.log(arguments);
  return true;
}

function persist(inputs) {
  console.log(arguments.callee.name); console.log(arguments);
  return Q.all([
      writeTree(inputs.tree),
      writeDependencies(inputs.dependencies)]
      ).then(
        function () {
          return inputs;
      });
}

function convertToDebian(inputs) {
  console.log(arguments.callee.name); console.log(arguments);
  return Q.all([
      writeMakefile(inputs.tree, inputs.dependencies), 
      debianize(inputs.tree, inputs.dependencies)]);
}

target = 'socket.io';
p = Q
  .call(readTreeDependencies, target, 'fn1', 'fn2')
  .then(
      function(inputs) {return inputs; }, 
      function(err) { return getTreeDependencies(target); }
      )
  .then(persist)
  .then(convertToDebian)
  .fail(function(err) { 
    console.log('failure handler: ' + err); 
    }
  );
