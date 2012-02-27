var lophilo = require('./build/default/lophilo');
lophilo.init();
lophilo.register(1, 1, 1000, function (source, value) {
  console.log("source %d update %d", source, value);
});
while(true) {
}
