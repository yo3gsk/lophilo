dumpversion = require('./dumpversion.js');
repl = require('repl');
Q = require('qq');

R = repl.start();
R.context.result = 'waiting for result';
dumpversion.getPackage('burrito',
  function(output) {
    console.log('result in variable result');
    R.context.result = output;
  });
