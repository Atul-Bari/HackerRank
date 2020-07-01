#include <iostream>
#include <stdio.h>
#include "hello.pb.h"
#include "hello.grpc.pb.h"

#include "hello-lib.h"
#include <string>
#include <grpc++/grpc++.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using grpc::StatusCode;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;
using hello::HelloLib;
using std::string;

class GreeterClient {

public:
        GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {
      std::cout << "class``";
      }

        std::string SayHello(const std::string& user) {
                // Data we are sending to the server.
                HelloRequest request;
                request.set_name(user);

                // Container for the data we expect from the server.
                HelloReply reply;

                // Context for the client. It could be used to convey extra information to
                // the server and/or tweak certain RPC behaviors.
                ClientContext context;

                // The actual RPC.
                Status status = stub_->SayHello(&context, request, &reply);

                // Act upon its status.
                if (status.ok()) {
                  return reply.message();
                } else {
                  std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
                  std::cout << status.error_message() << std::endl;
                // lets print the error code, which is 3
                std::cout << status.error_code() << std::endl;
                // want to do some specific action based on the error?
                if(status.error_code() == StatusCode::INVALID_ARGUMENT) {
                // do your thing here}
                }
                  return "RPC failed";
                }
        }
private:
        std::unique_ptr<Greeter::Stub> stub_;
};


int main() {
        std::cout << "Main";
	HelloLib lib("Hello");
  string thing = "world";
  if (argc > 1) {
    thing = argv[1];
  }
  lib.greet(thing);
        GreeterClient greeter(grpc::CreateChannel(
        "localhost:50051", grpc::InsecureChannelCredentials()));
        std::string user("Atul");
        std::string reply = greeter.SayHello(user);
        std::cout << "Greeter received: " << reply << std::endl;

}
 
