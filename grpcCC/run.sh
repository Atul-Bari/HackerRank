protoc -I . --cpp_out=. hello.proto;
protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` hello.proto;

g++ -std=c++11 -fPIC   -c -o hello.pb.o hello.pb.cc
g++ -std=c++11 -fPIC   -c -o hello.grpc.pb.o hello.grpc.pb.cc
g++ -std=c++11 -fPIC   -c -o client.o client.cc

g++ hello.pb.o hello.grpc.pb.o  -ggdb -o wrap server.cc -std=c++1z -rdynamic -pthread -static-libstdc++ -Wl,-non_shared  -lprotobuf -pthread -lz -lgrpc++ -lprotobuf -lgrpc -lz -lcares -lssl -lcrypto -lunwind -llzma -lgflags -Wl,-call_shared -lgrpc++_reflection -lpthread -ldl -Wl,-Bsymbolic-functions -o s_server

g++ hello.pb.o hello.grpc.pb.o  -ggdb -o wrap client.cc -std=c++1z -rdynamic -pthread -static-libstdc++ -Wl,-non_shared  -lprotobuf -pthread -lz -lgrpc++ -lprotobuf -lgrpc -lz -lcares -lssl -lcrypto -lunwind -llzma -lgflags -Wl,-call_shared -lgrpc++_reflection -lpthread -ldl -Wl,-Bsymbolic-functions -o s_client

# g++ -fPIC -shared -std=c++11 -rdynamic -pthread -static-libstdc++ -Wl,-non_shared  -lprotobuf -pthread -lz -lgrpc++ -lprotobuf -lgrpc -lz -lcares -lssl -lcrypto -lunwind -llzma -lgflags -Wl,-call_shared -lgrpc++_reflection -lpthread -ldl -Wl,-Bsymbolic-functions -o libhello.so hello.grpc.pb.o hello.pb.o
