#include "converters.h"
#include "v8_context.h"

#include<stdio.h>

using namespace v8;

v8_context::v8_context() : handle(Context::New()) {}

v8_context::~v8_context() {
  handle.Dispose();
}

VALUE v8_context_allocate(VALUE clazz) {
  v8_context *cxt = new v8_context;
  return Data_Wrap_Struct(clazz, v8_context_mark, v8_context_free, cxt);
  
}
void v8_context_free(v8_context *context) {
  delete context;
}
void v8_context_mark(v8_context *context) {
  //don't mark anything.
}


//methods
VALUE v8_context_eval(VALUE self, VALUE javascript) {
  v8_context* cxt = 0;
  Data_Get_Struct(self, struct v8_context, cxt);
  
  Context::Scope enter(cxt->handle);
  HandleScope handles;
  
  RubyValueSource<StringDest, std::string> tostring;
  const std::string source(tostring(javascript));
  
  Local<Script> script = Script::Compile(String::New(source.c_str()));
  Local<Value> result = script->Run();
  convert_v8_to_rb_t toValue;
  return toValue(result);
}

VALUE v8_context_inject(VALUE self, VALUE key, VALUE value) {
    
    v8_context* context = 0;
    Data_Get_Struct(self, struct v8_context, context);
    
    Context::Scope enter(context->handle);
    HandleScope handles;
  
    convert_rb_to_string_t tostring;  
    convert_rb_to_v8_t toHandle;

    const std::string key_string(tostring(key));
        
    // does this need to be a persistent handle ?
    Local<Value> key_handle(String::New(key_string.c_str()));
    Local<Value> value_handle(toHandle(value));

    Local<Object> global = context->handle->Global();
    global->Set(key_handle, value_handle);

    return value;
}

