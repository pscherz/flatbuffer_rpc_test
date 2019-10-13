#include <iostream>
#include <string>

#include <rpc_generated.h>
#include <zmq.hpp>

namespace fbs = flatbuffers;

int32_t server_count;

/// appends ' -- HELLO WORLD!' to the request string
fbs::Offset<HelloResponse> handle_hellorequest(const HelloRequest *hr,
                                               fbs::FlatBufferBuilder &fbb) {
  if (!hr) {
    return {};
  }

  std::string r = hr->content()->c_str();
  r += " -- HELLO WORLD!";
  return CreateHelloResponseDirect(fbb, r.c_str());
}

/// increments server_count by the requested amount
fbs::Offset<AddResponse> handle_addrequest(const AddRequest *ar,
                                           fbs::FlatBufferBuilder &fbb) {
  if (!ar) {
    return {};
  }

  server_count += ar->count();
  return CreateAddResponse(fbb, server_count);
}

/// Handle ZeroMQ message and generate a reply
void handle(zmq::message_t &msg, fbs::FlatBufferBuilder &fbb) {
  const MyRequest *request = GetMyRequest(msg.data());

  fbs::Offset<HelloResponse> hr =
      handle_hellorequest(request->hellorequest(), fbb);
  fbs::Offset<AddResponse> ar = handle_addrequest(request->addrequest(), fbb);

  MyRequestBuilder mrb(fbb);
  mrb.add_helloresponse(hr);
  mrb.add_addresponse(ar);

  FinishMyRequestBuffer(fbb, mrb.Finish());

  std::cout << "SERVER: created message of size: " << fbb.GetSize()
            << std::endl;
}

int main() {
  zmq::context_t ctx;
  zmq::socket_t sock(ctx, zmq::socket_type::rep);

  sock.bind("tcp://*:50001");

  zmq::message_t msg;
  for (;;) {
    sock.recv(msg);

    fbs::FlatBufferBuilder fbb;
    handle(msg, fbb);

    sock.send(zmq::buffer(fbb.GetBufferPointer(), fbb.GetSize()),
              zmq::send_flags::dontwait);
  }
}

// vim: sw=2 ts=2 expandtab smarttab
