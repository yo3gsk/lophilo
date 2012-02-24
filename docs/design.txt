
= target: a simple oscilloscope =

lophilo hardware analog pin 1 is connected to a datasource generating a sinewave
lophilo ADC and FPGA continuously read the data and updates memory location
(above is simulated using QEMU)
lophilo kernel driver reads value and notifies user-space of updated value at pre-specified intervals
	http://www.ibm.com/developerworks/linux/library/l-timers-list/index.html?ca=drs-
	http://people.ee.ethz.ch/~arkeller/linux/kernel_user_space_howto.html
v8/node receive and handle notification by calling Javascript callback
node server Javascript callback emits events using Socket.IO
Browser Javascript graphically displays the results

= code example =

== Javascript code ==

var lophilo = require('lophilo');
function readFromPin0(value) {
	//sendData to UI client
	console.log(value);
}

lophilo.analogRead(lophilo.PIN0, readFromPin0, 50); 