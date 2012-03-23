function parentfnc(param) {
  fnc = function childfnc() {
    return param;
  }
  return fnc;
}

function thirdpartyfnc(fnc) {
  return fnc;
}

function parentfnc2(param) {
  fnc = thirdpartyfnc(function childfnc() {
    return param;
  });
  return fnc;
}

console.log(parentfnc('something')());
console.log(parentfnc2('something too')());
