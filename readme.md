# RPC using Flatbuffers

This is a test using a hand-rolled RPC mechanism using Flatbuffer messages over
ZeroMQ sockets.

Flatbuffers makes it easy to check, if a part of a message is actually used and
so I thought, you could use a message with all requests you understand and just
fill in those you actually want to call.

## Build

Needs cmake and vcpkg with installed flatbuffers and cppzmq.

## Usage

Server needs no arguments, client takes one argument, that it uses as id.

The client ids 0 and 1 are special, 0 send no requests, 1 sends only an
AddRequest.

