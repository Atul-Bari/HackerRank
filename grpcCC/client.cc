#include <iostream>
#include <stdio.h>
#include "hello.pb.h"
#include "hello.grpc.pb.h"
#include "client.h"
#include <grpc++/grpc++.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using grpc::StatusCode;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

class GreeterClient {
public:
	GreeterClient(std::shared_ptr<Channel> channel);
	std::string SayHello();
private:
	std::unique_ptr<Greeter::Stub> stub_;
};

GreeterClient::GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {
      std::cout << "class``";
      }

        std::string GreeterClient::SayHello() {
                // Data we are sending to the server.
		std::string user("Atul");
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


#ifdef __cplusplus
extern "C" {
#endif

// Inside this "extern C" block, I can implement functions in C++, which will externally 
//   appear as C functions (which means that the function IDs will be their names, unlike
//   the regular C++ behavior, which allows defining multiple functions with the same name
//   (overloading) and hence uses function signature hashing to enforce unique IDs),


static GreeterClient  *AAA_instance = NULL;

void lazyAAA() {
    if (AAA_instance == NULL) {
        AAA_instance = new GreeterClient(grpc::CreateChannel(
        "localhost:50051", grpc::InsecureChannelCredentials()));
    }
}

void AAA_sayHi(const char *name) {
    lazyAAA();
    std::string s=AAA_instance->SayHello();
}

#ifdef __cplusplus
}
#endif

/*

int main() {
        std::cout << "Main";
        GreeterClient greeter(grpc::CreateChannel(
        "localhost:50051", grpc::InsecureChannelCredentials()));
        std::string user("Atul");
        std::string reply = greeter.SayHello(user);
        std::cout << "Greeter received: " << reply << std::endl;

}
 */
