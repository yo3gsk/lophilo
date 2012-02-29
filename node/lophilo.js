var fs = require('fs');
var assert = require("assert");
var util = require('util'),
    events = require('events');

var lophilo_configuration = {};

function Lophilo() {
    if(false === (this instanceof Lophilo)) {
        return new Lophilo();
    }

    events.EventEmitter.call(this);
}
util.inherits(Lophilo, events.EventEmitter);

Lophilo.prototype.configure = function(configuration) {
	lophilo_configuration = { 'last_update_sec_id': 1, 'last_update_usec_id': 2, 'pin_xa0_id': 3 };
	this.watch('/dev/lophilo');
	return lophilo_configuration;
}

Lophilo.prototype.watch = function(device) {
	var self = this;
	var readStream = fs.createReadStream(device); 
	readStream.on('data', function(data){ 
		var updates = [];
		MESSAGE_LENGTH = 8;
		assert((data.length % MESSAGE_LENGTH) == 0);
		for (var i=0; i<data.length; i += MESSAGE_LENGTH) {
			source = data[i] | data[i+1]<<8 | data[i+2]<<16 | data[i+3]<<24;
			value = data[i+4] | data[i+5]<<8 | data[i+6]<<16 | data[i+7]<<24;
			//console.log("source %d value %d", source, value);
			updates.push({ 'source': source, 'value': value });
		}
		self.emit('update', updates);
	}); 
	readStream.on('end', function(){ 
		console.log('Stream END'); 
	}); 
}
module.exports = Lophilo;
