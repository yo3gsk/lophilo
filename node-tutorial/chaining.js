Q = require('qq');
http = require('http');

function hello() 
{
  console.log('creating promise');
  var deferred = Q.defer();
  setTimeout(function() {
      console.log('timeout!');
      console.log('resolving promise');
      deferred.resolve(true);
  }, 1000);
  console.log('returning promise');
  return deferred.promise;
}

Q.call(hello)
  .then(hello)
  .then(hello)
  .then(function() {
    console.log('bye bye bye');
  }, 
  function() { 
    console.log('error!'); 
  }).end();

http.createServer(function (req, res) {}).listen(process.env.PORT || 8080, "0.0.0.0");
