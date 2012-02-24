var lophilo = require('./build/default/lophilo');
console.log(lophilo);
function readFromPin0(value) {
	console.log(value);
}

lophilo.analogRead(readFromPin0, lophilo.PIN0, 50); 
