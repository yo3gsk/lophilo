// npm install -g supervisor
// supervisor -w test.js -p test.js
var Lophilo = require('./lophilo');
var http = require('http');
var app = http.createServer(handler);
var io = require('socket.io').listen(app);
var static = require('node-static');
var fileServer = new static.Server('./public');

lophilo = new Lophilo();
var lophilo_configuration = lophilo.configure({});

function handler (request, response) {
  request.addListener('end', function () {
    fileServer.serve(request, response);
  });
}

io.set('log level', 0);                    // reduce logging
io.sockets.on('connection', function (socket) {
  socket.emit('lophilo_configuration', lophilo_configuration);
  lophilo.on('update', function(data) {
    socket.emit('lophilo_update', data);
  });
  socket.on('client', function (data) {
    console.log(data);
  });
});

app.listen(8001);
