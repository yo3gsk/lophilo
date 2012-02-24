var http = require('http');
ip = "10.236.10.145";
http.createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.end('Hello World\n');
}).listen(1337, ip);
console.log('Server running at http://' + ip + ':1337/');
