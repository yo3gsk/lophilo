extern "C" {
	#include "lophilo.h"
	#include "client.h"
}

#include <v8.h>
#include <node.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>
#include <pthread.h>

using namespace std;
using namespace node;
using namespace v8;

static Handle<Value> Init(const Arguments& args);

static Handle<Value> RegisterAsync (const Arguments&);
static int WaitRegister (eio_req *);
static int WaitRegister_After (eio_req *);
extern "C" void init (Handle<Object>);

static void UpdateCallback(EV_P_ ev_async *watcher, int revents);

static ev_async eio_notifier;

static pthread_t msg_thread;

void update(int source, int value);
Persistent<Object> module_handle;

struct simple_request {
  lophilo_sources_types type;
  lophilo_sources source;
  int period;
  Persistent<Function> cb;
};

map<lophilo_sources, Persistent<Function> > *source_to_cb = new map<lophilo_sources, Persistent<Function> >;

static void* MsgThread(void *)
{
  lophilo_loop();
}

static Handle<Value> Init(const Arguments& args) {
  HandleScope scope;
  const char *usage = "usage: init()";
  printf("init called from Javascript!\n");
  if (args.Length() != 0) {
    return ThrowException(Exception::Error(String::New(usage)));
  }
  ev_async_init(&eio_notifier, UpdateCallback);
  ev_async_start(EV_DEFAULT_UC_ &eio_notifier);
  ev_unref(EV_DEFAULT_UC);
  pthread_create(&msg_thread, NULL, MsgThread, 0);
  return Undefined();
}


static Handle<Value> RegisterAsync (const Arguments& args) {
  HandleScope scope;
  const char *usage = "usage: register(source, value, period, cb)";
  if (args.Length() != 4) {
    return ThrowException(Exception::Error(String::New(usage)));
  }
  int type = args[0]->Int32Value();
  int source = args[1]->Int32Value();
  int period = args[2]->Int32Value();
  Local<Function> cb = Local<Function>::Cast(args[3]);

  simple_request *sr = (simple_request *)
    malloc(sizeof(struct simple_request) + 1);

  sr->type = (lophilo_sources_types)type;
  sr->source = (lophilo_sources)source;
  sr->period = period;
  sr->cb = Persistent<Function>::New(cb);

  eio_custom(WaitRegister, EIO_PRI_DEFAULT, WaitRegister_After, sr);
  ev_ref(EV_DEFAULT_UC);
  return Undefined();
}

// this function happens on the thread pool
// doing v8 things in here will make bad happen.
static int WaitRegister (eio_req *req) {
  struct simple_request * sr = (struct simple_request *)req->data;
  lophilo_register(sr->type, sr->source, sr->period);
  source_to_cb->insert(std::make_pair(sr->source, sr->cb));
  free(sr);
  return 0;
}

static int WaitRegister_After (eio_req *req) {
  return 0;
}

int source;
int value;

static void UpdateCallback(EV_P_ ev_async *watcher, int revents)
{
  HandleScope scope;

  assert(watcher == &eio_notifier);
  assert(revents == EV_ASYNC);

  printf("update!\n");
  Local<Value> argv[2];
  argv[0] = Integer::New(source);
  argv[1] = Integer::New(value);
  map<lophilo_sources, Persistent<Function> >::iterator iter = source_to_cb->find((lophilo_sources)source);
  if(iter != source_to_cb->end()) {
	  printf("calling function!\n");
	  TryCatch try_catch;
	    //((*iter).second)->Call(Context::GetCurrent()->Global(), 2, argv);
	    ((*iter).second)->Call(module_handle, 2, argv);
	  if (try_catch.HasCaught()) {
	    FatalException(try_catch);
	  } 
  }
  printf("updated...\n");
}

void update(int s, int v) {
  printf("sending ev_async_send\n");
  source = s;
  value = v;
  ev_async_send(EV_DEFAULT_UC_ &eio_notifier);
  return;
}

extern "C" void init (Handle<Object> target) {
  HandleScope scope;
  lophilo_init(&update);
  NODE_SET_METHOD(target, "init", Init);
  NODE_SET_METHOD(target, "register", RegisterAsync);
  module_handle = Persistent<Object>::New(target);
}
