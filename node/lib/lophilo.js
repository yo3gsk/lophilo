var lophilo = require('lophilo');
start = Date.now()

console.log("js "+(Date.now() - start), "before hello")
lophilo.doSomething(1, 2, "hello", function (er, res, n) {
  console.log("js "+(Date.now() - start), er, res, n)
})
console.log("js "+(Date.now() - start), "before goodbye")
lophilo.doSomething(3, 4, "goodbye", function (er, res, n) {
  console.log("js "+(Date.now() - start), er, res, n)
})
