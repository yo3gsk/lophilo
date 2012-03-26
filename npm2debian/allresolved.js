Q = require('./q');

function success() {
  return true;
}

function failure() {
  throw new Error('simulated error!');
}
Q.all([Q.call(success), Q.call(failure)]).then(
    function h() {
      console.log('with all and one failure');
    }
  );
Q.all([Q.call(success), Q.call(success)]).then(
    function h() {
      console.log('with all and all successes');
    }
  );
Q.allResolved([Q.call(success), Q.call(failure)]).then(
    function h() {
      console.log('with allResolved and one failure');
    }
  );
