/usr/bin/protoc -I . --cpp_out=. say.proto;
/usr/bin/protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` say.proto;

g++ -std=c++11 -fPIC  -c -o say.pb.o say.pb.cc && g++ -std=c++11 -fPIC -c -o say.grpc.pb.o say.grpc.pb.cc && g++ -std=c++11 -I/opt/samespace/media/freeswitch/include/freeswitch/  -I/usr/local/include  -I/usr/local/src/freeswitch/src/include -I/usr/local/include  -I/usr/local/src/freeswitch/src/include -I/usr/local/src/freeswitch/libs/libteletone/src  -fPIC -c -o Grpc_client.o speechInterface.cpp

echo dependancy compiled

#gcc -fPIC -shared -std=c++11 -lprotobuf -lgrpc++ -lgrpc -lz  -lssl -lcrypto -Wl,-call_shared -lpthread -ldl -lgrpc++_reflection -lltdl -o libspeechMod.so say.pb.o say.grpc.pb.o Grpc_client.o


#gcc -fPIC -shared -std=c++11 -rdynamic -pthread -static-libstdc++ -Wl,-non_shared  -lprotobuf -pthread -lz -lgrpc++ -lprotobuf -lgrpc -lz -lcares -lssl -lcrypto -lunwind -llzma -lgflags -Wl,-call_shared -lgrpc++_reflection -lpthread -ldl -Wl,-Bsymbolic-functions -o libspeechMod1.so say.pb.o say.grpc.pb.o Grpc_client.o

gcc -fPIC -shared -std=c++1z -Wl,-non_shared  -lprotobuf -lgrpc++  -lgrpc -lz -lcares -lssl -lcrypto -Wl,-call_shared -lpthread -ldl -lgrpc++_reflection -lstdc++ -lm -lgpr -Wl,--hash-style=both  -o libspeechMod.so say.pb.o say.grpc.pb.o Grpc_client.o


#gcc -fPIC -shared  -ggdb  -std=c++11  -rdynamic -pthread -static-libstdc++ -Wl,-non_shared  -lprotobuf -pthread -lz -lgrpc++ -lprotobuf -lz -lcares -lssl -lcrypto -lunwind -llzma -lgflags -Wl,-call_shared -lgrpc++_reflection -lpthread -ldl  -Wl,-Bsymbolic-functions  -o libspeechMod2.so say.pb.o say.grpc.pb.o Grpc_client.o

#gcc -fPIC -shared -std=c99 -rdynamic -pthread -static-libstdc++ -Wl,-non_shared  -pthread -lz -lgrpc++ -lprotobuf -lgrpc -lz -lcares -lssl -lcrypto -lunwind -llzma -lgflags -Wl,-call_shared -lpthread -ldl  -o libspeechMod2.so say.pb.o say.grpc.pb.o Grpc_client.o

#gcc -fPIC -shared -std=c++11 -rdynamic -pthread -static-libstdc++ -Wl,-non_shared  -lprotobuf -pthread -lz -lgrpc++ -lprotobuf -lgrpc -lz -lcares -lssl -lcrypto -lunwind -llzma -lgflags -Wl,-call_shared -lgrpc++_reflection -lpthread -ldl -Wl,-Bsymbolic-functions -o libspeechMod.so say.pb.o say.grpc.pb.o Grpc_client.o

cp libspeechMod.so /usr/local/lib
ldconfig
# -lgrpc++_reflection
echo library generated and copied
# gcc -fPIC -shared -std=c++11 -lprotobuf -lgrpc++ -lgrpc -lz -lcares -lssl -lcrypto -Wl,-call_shared -lpthread -ldl -lgrpc++_reflection -lltdl -o libspeechMod.so say.pb.o say.grpc.pb.o Grpc_client.o

#-rdynamic -pthread -static-libstdc++ -Wl,-non_shared  -lprotobuf -pthread -lz -lgrpc++ -lprotobuf -lgrpc -lz -lcares -lssl -lcrypto -lunwind -llzma -lgflags -Wl,-call_shared -lgrpc++_reflection -lpthread -ldl -Wl,-Bsymbolic-functions
