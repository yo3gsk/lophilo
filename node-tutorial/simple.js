Q = require('qq');
function hello() {
  console.log(arguments.callee.name); console.log(arguments);
  return [1, 2];
}
function world(a, b) {
  console.log(arguments.callee.name); console.log(arguments);
  return [a+b, a+b];
}

function bye(a, b) {
  console.log(arguments.callee.name); console.log(arguments);
  return [a,b];
}

v = Q.step(hello, world, bye);
console.log(v);
