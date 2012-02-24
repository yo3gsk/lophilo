#define BUILDING_NODE_EXTENSION
#include <node.h>

using namespace v8;

static Persistent<Function> cb;

void readData() {
  cb->Call(Context::GetCurrent()->Global(), argc, argv);
}

Handle<Value> AnalogRead(const Arguments& args) {
  HandleScope scope;

  cb = Persistent<Function>::Cast(args[0]);
  Local<Value> argv[argc] = { Local<Value>::New(String::New("hello world")) };

  return scope.Close(Undefined());
}

void Init(Handle<Object> target) {
  target->Set(String::NewSymbol("analogRead"),
      FunctionTemplate::New(AnalogRead)->GetFunction());
}

NODE_MODULE(lophilo, Init)
