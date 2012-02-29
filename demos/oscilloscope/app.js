// npm install -g supervisor
// supervisor -w app.js -p app.js
var PORT=8001 // set to 0 for dynamically allocated port
var Lophilo = require('lophilo');
var http = require('http');
var app = http.createServer(handler);
var io = require('socket.io').listen(app);
var static = require('node-static');
var fileServer = new static.Server('./public');
var os=require('os');
var ifaces=os.networkInterfaces();
var dns = require('dns');

lophilo = new Lophilo();
var lophilo_configuration = lophilo.configure({});

function handler (request, response) {
  request.addListener('end', function () {
    fileServer.serve(request, response);
  });
}

io.set('log level', 0);                    // reduce logging
io.sockets.on('connection', function (socket) {
  console.log('new socket: %s:%s', socket.address, socket.port);
  socket.emit('lophilo_configuration', lophilo_configuration);
  lophilo.on('update', function(data) {
    socket.emit('lophilo_update', data);
  });
  socket.on('client', function (data) {
    console.log(data);
  });
});

app.listen(PORT, function() {
	for (var dev in ifaces) {
	  var alias=0;
	  ifaces[dev].forEach(function(details){
	    if (details.family=='IPv4') {
	      console.log('http://%s:%d/client.html', details.address, app.address()['port']);
	      ++alias;
	    }

	    dns.reverse(details.address, function(err, domains) {
      	      if (err) {
		console.log('reverse for ' + details.address + ' failed: ' +
		  err.message);
	      } else {
		console.log('reverse for ' + details.address + ': ' +
		  JSON.stringify(domains));
	      }
            });
	  });
	}
});
