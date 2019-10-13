#include <iostream>
#include <string>

#include <rpc_generated.h>
#include <zmq.hpp>

namespace fbs = flatbuffers;

void handle_request(const std::string &id, fbs::FlatBufferBuilder &fbb) {
  std::string m = "ho! from ";
  m += id;
  fbs::Offset<HelloRequest> hellorequest;
  fbs::Offset<AddRequest> addrequest;

  // client with id "0" should send an empty request
  if (id != "0") {
    // client with id "1" should send no HelloRequest
    if (id != "1") {
      hellorequest = CreateHelloRequestDirect(fbb, m.c_str());
    }
    addrequest = CreateAddRequest(fbb, 1);
  }

  auto request = CreateMyRequest(fbb, hellorequest, {}, addrequest, {});
  FinishMyRequestBuffer(fbb, request);
}

void handle_response(const zmq::message_t &message) {
  const MyRequest *response = GetMyRequest(message.data());
  const HelloResponse *hrr = response->helloresponse();
  if (hrr) {
    std::cout << "CLIENT: " << hrr->content()->c_str() << std::endl;
  }

  const AddResponse *arr = response->addresponse();
  if (arr) {
    std::cout << "CLIENT: Server counted up to " << arr->current_count()
              << std::endl;
  }
}

int main(int argc, char **argv) {
  zmq::context_t ctx;
  zmq::socket_t sock(ctx, zmq::socket_type::req);

  sock.connect("tcp://localhost:50001");

  std::string id = "<no name specified>";
  if (argc > 1) {
    id = argv[1];
  }

  fbs::FlatBufferBuilder fbb;
  handle_request(id, fbb);

  sock.send(zmq::buffer(fbb.GetBufferPointer(), fbb.GetSize()));

  std::cout << "CLIENT: (" << id << ") sent message of size: " << fbb.GetSize()
            << std::endl;

  zmq::message_t message;
  sock.recv(message);

  handle_response(message);
}

// vim: sw=2 ts=2 expandtab smarttab
