// npm install -g supervisor
// supervisor -w test.js -p test.js
var fs = require('fs');
var util = require('util');
var app = require('http').createServer(handler);
var io = require('socket.io').listen(app);
// production settings
io.configure('production', function(){
	io.enable('browser client minification');  // send minified client
	io.enable('browser client etag');          // apply etag caching logic based on version number
	io.enable('browser client gzip');          // gzip the file
	io.set('log level', 0);                    // reduce logging
	io.set('transports', [                     // enable all transports (optional if you want flashsocket)
	    'websocket'
	  , 'flashsocket'
	  , 'htmlfile'
	  , 'xhr-polling'
	  , 'jsonp-polling'
	]);
});

io.set('log level', 0);                    // reduce logging
io.configure('development', function(){
	io.enable('browser client minification');  // send minified client
	io.enable('browser client etag');          // apply etag caching logic based on version number
	io.enable('browser client gzip');          // gzip the file
	io.set('log level', 0);                    // reduce logging
	io.set('transports', [                     // enable all transports (optional if you want flashsocket)
	    'websocket'
	  , 'flashsocket'
	  , 'htmlfile'
	  , 'xhr-polling'
	  , 'jsonp-polling'
	]);
});

var static = require('node-static');
var fileServer = new static.Server('./public');

app.listen(8001);

function handler (request, response) {
  request.addListener('end', function () {
    fileServer.serve(request, response);
  });
}

io.sockets.on('connection', function (socket) {
	var readStream = fs.createReadStream("/dev/lophilo"); 
	readStream.on('data', function(data){ 
		for (var i=0; i<data.length; i+=4) {
			counter = data[i] | data[i+1]<<8 | data[i+2]<<16 | data[i+3]<<24;
			//util.print(counter);
			//util.print(" ");
			socket.emit('lophilo', { 'counter': counter });
		}
	}); 
	readStream.on('end', function(){ 
		console.log('Stream END'); 
	}); 
  socket.on('client', function (data) {
    console.log(data);
  });
});

