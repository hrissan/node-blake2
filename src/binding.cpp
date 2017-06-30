#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <nan.h>
#include <cstddef>
#include <cassert>
#include <cstring>

#include "blake2.h"

static void quick_blake2b(int digest_length, const uint8_t * buffer_data, int buffer_length, uint8_t * digest){
 
 blake2b_state state;
 blake2b_init(&state, digest_length);
 
 blake2b_update(&state, buffer_data, buffer_length);
 
 blake2b_final(&state, digest, digest_length);
}
 
void blake2b_n(const v8::FunctionCallbackInfo<v8::Value>& info) {
 if( info.Length() < 2 )
 return Nan::ThrowError("blake2b_n must have 2 args");
 if (!node::Buffer::HasInstance(info[0])) {
 return Nan::ThrowError(v8::Exception::TypeError(Nan::New<v8::String>("Bad first argument; need a Buffer").ToLocalChecked()));
 }
 if (!info[1]->IsNumber()) {
 return Nan::ThrowError(v8::Exception::TypeError(Nan::New<v8::String>("Bad second argument; must be a number").ToLocalChecked()));
 return;
 }
 v8::Local<v8::Object> buffer_obj = info[0]->ToObject();
 const char *buffer_data = node::Buffer::Data(buffer_obj);
 size_t buffer_length = node::Buffer::Length(buffer_obj);
 
 double n_rep = info[1]->NumberValue();
 int digest_length = 32;//BLAKE2B_OUTBYTES;
 unsigned char digest[512 / 8];
 quick_blake2b(digest_length, reinterpret_cast<const uint8_t*>(buffer_data), buffer_length, digest);
 n_rep -= 1;
 while(n_rep > 0){
 quick_blake2b(digest_length, digest, digest_length, digest);
 n_rep -= 1;
 }
 
 v8::Local<v8::Value> rc = Nan::Encode(
 reinterpret_cast<const char*>(digest),
 digest_length,
 Nan::BUFFER
 );
 
 info.GetReturnValue().Set(rc);
 }
 
 static void
 init(v8::Local<v8::Object> target) {
 NODE_SET_METHOD(target, "blake2b_n", blake2b_n);
 }
 
 NODE_MODULE(binding, init)

