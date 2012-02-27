// npm install -g supervisor
// supervisor -w test.js -p test.js
var fs = require('fs');
var util = require('util');
var app = require('http').createServer(handler);
var io = require('socket.io').listen(app);

app.listen(8001);

function handler (req, res) {
  fs.readFile(__dirname + '/client.html',
  function (err, data) {
    if (err) {
      res.writeHead(500);
      return res.end('Error loading client.html');
    }

    res.writeHead(200, {"Content-Type" : "text/html"});
    res.end(data);
  });
}

io.sockets.on('connection', function (socket) {
	var readStream = fs.createReadStream("/dev/lophilo"); 
	readStream.on('data', function(data){ 
		for (var i=0; i<data.length; i+=4) {
			counter = data[i] | data[i+1]<<8 | data[i+2]<<16 | data[i+3]<<24;
			util.print(counter);
			util.print(" ");
			socket.emit('lophilo', { 'counter': counter });
		}
	}); 
	readStream.on('end', function(){ 
		console.log('Stream END'); 
	}); 
  socket.on('my other event', function (data) {
    console.log(data);
  });
});

