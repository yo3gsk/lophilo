var fs = require('fs');
var util = require('util');
var readStream = fs.createReadStream("/dev/lophilo"); 
readStream.on('data', function(data){ 
	console.log(util.inspect(data));
	for (var i=0; i<data.length; i+=4) {
		console.log("%d ", data[i] | data[i+1]<<8 | data[i+2]<<16 | data[i+3]<<24);
	}
}); 
readStream.on('end', function(){ 
	console.log('Stream END'); 
}); 
