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
#include <uv.h>

using namespace std;
using namespace node;
using namespace v8;

static Handle<Value> Init(const Arguments& args);
static Handle<Value> RegisterAsync (const Arguments&);
static void wait_register (uv_work_t *);
static void wait_register_after (uv_work_t *);
extern "C" void init (Handle<Object>);

static pthread_t msg_thread;

static void update(int source, int value);

typedef struct lophilo_request {
  lophilo_sources_types type;
  lophilo_sources source;
  int period;
  Persistent<Function> cb;
} lophilo_request_t;

typedef struct lophilo_update {
  int source;
  int value;
} lophilo_update_t;

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
  pthread_create(&msg_thread, NULL, MsgThread, 0);
  return True();
}


static Handle<Value> RegisterAsync (const Arguments& args) {
  HandleScope scope;
  const char *usage = "usage: register(source, value, period, cb)";
  if (args.Length() != 4) {
    return ThrowException(Exception::Error(String::New(usage)));
  }

  lophilo_request *sr = new lophilo_request();

  sr->type = (lophilo_sources_types) args[0]->Int32Value();
  sr->source = (lophilo_sources) args[1]->Int32Value();
  sr->period = args[2]->Int32Value();
  sr->cb = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_work_t* work = new uv_work_t();
  work->data = sr;
  printf("%x\n", uv_default_loop());
  uv_queue_work(uv_default_loop(), work, wait_register, wait_register_after);
  return True();
}

// this function happens on the thread pool
// doing v8 things in here will make bad happen.
static void wait_register (uv_work_t *req) {
  printf("background registration\n");
  struct lophilo_request * sr = (struct lophilo_request *)req->data;
  lophilo_register(sr->type, sr->source, sr->period);
  source_to_cb->insert(std::make_pair(sr->source, sr->cb));
  delete sr;
}

static void wait_register_after (uv_work_t *work) {
  delete work;
}

static void async(uv_work_t* work) 
{
  //we're getting called back, we just want to call our method...
  printf("async computation...\n");
}

static void after(uv_work_t* work) 
{
  printf("calling JS callback!\n");
  HandleScope scope;
  printf("Scope handle created!\n");
  lophilo_update_t* update = (lophilo_update_t*) work->data;
  Handle<Value> argv[2];
  argv[0] = Integer::New(update->source);
  argv[1] = Integer::New(update->value);
  map<lophilo_sources, Persistent<Function> >::iterator iter = source_to_cb->find((lophilo_sources)update->source);
  if(iter != source_to_cb->end()) {
	  printf("calling function!\n");
	  TryCatch try_catch;
	    ((*iter).second)->Call(Context::GetCurrent()->Global(), 2, argv);
	  if (try_catch.HasCaught()) {
	    FatalException(try_catch);
	  } 
  }
  delete update;
  delete work;
}

static void update(int s, int v) 
{
  printf("update received, queueing work\n");
  uv_work_t *req = new uv_work_t();
  lophilo_update_t* update = new lophilo_update_t();
  update->source = s;
  update->value = v;
  req->data = update;
  printf("%x\n", uv_default_loop());
  uv_queue_work(uv_default_loop(), req, async, after);
  return;
}

extern "C" void init (Handle<Object> target) {
  HandleScope scope;
  lophilo_init(&update);
  NODE_SET_METHOD(target, "init", Init);
  NODE_SET_METHOD(target, "register", RegisterAsync);
}
