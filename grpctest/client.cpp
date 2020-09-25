#include <cstdlib>
//#include <switch.h>

#include <stdlib.h>
#include <string.h>
//#include "speechInterface.h"
//#include <switch_buffer.h>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <thread>
#include <cstdint>
#include <sstream>
#include <grpc++/grpc++.h>
#include "say.pb.h"
#ifdef BAZEL_BUILD
#include "examples/protos/say.grpc.pb.h"
#else
#include "say.grpc.pb.h"
#endif

using namespace std;

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using say::Speech;
using say::Text;
using say::TextToSpeech;
using say::AsrSpeech;

class speechModule
{
public:
        speechModule(std::string clientId, std::string ip_port);
        std::string speech_tts();
        //std::string speech_sst(std::string data, char* voiceName);

private:
        std::unique_ptr<TextToSpeech::Stub> stub_;
};

class MyCustomAuthenticator : public grpc::MetadataCredentialsPlugin
{
public:
        MyCustomAuthenticator(const grpc::string &ticket) : ticket_(ticket) {}

        grpc::Status GetMetadata(
            grpc::string_ref service_url, grpc::string_ref method_name,
            const grpc::AuthContext &channel_auth_context,
            std::multimap<grpc::string, grpc::string> *metadata) override
        {
                metadata->insert(std::make_pair("authorization", ticket_));
                return grpc::Status::OK;
        }

private:
        grpc::string ticket_;
};


Text MakeText()
{

        Text n;
        n.set_text("hi");
        n.set_voicename("en-US");
        return n;
}

speechModule::speechModule(std::string clientId, std::string ip_port)
{

 //       switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Connection request to server [%s]",ip_port.c_str());
 	printf("Connection request to server [%s]",ip_port.c_str());
        std::string BearerToken = "Bearer " + clientId;
        auto call_creds = grpc::MetadataCredentialsFromPlugin(
            std::unique_ptr<grpc::MetadataCredentialsPlugin>(
                new MyCustomAuthenticator(BearerToken)));

        auto channel_credentials = grpc::SslCredentials(grpc::SslCredentialsOptions());
        auto credentials = grpc::CompositeChannelCredentials(channel_credentials, call_creds);
	//auto credentials = grpc::CompositeChannelCredentials(grpc::InsecureChannelCredentials(), call_creds);
        //grpc::InsecureChannelCredentials()
	//stub_ = (TextToSpeech::NewStub(grpc::CreateChannel(ip_port, grpc::InsecureChannelCredentials())));
	stub_ = (TextToSpeech::NewStub(grpc::CreateChannel(ip_port, credentials)));
}

std::string speechModule::speech_tts()
{
   //     switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "API Test to Speech invoked!!!");
        Text request;
        Speech reply;
	printf("\nAPI Test to Speech invoked!!!");
        request = MakeText();

        char *buff;
        ClientContext context;
        Status status = stub_->Say(&context, request, &reply);

        if (status.ok())
        {
                return reply.audio();
        }
        else
        {
     //           switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[Error code: message] -> [%d : %s]\n", status.error_code(), (status.error_message()).c_str());
     		printf("[Error code: message] -> [%d : %s]\n", status.error_code(), (status.error_message()).c_str());
                return "Failed";
        }
}

int main(){
	printf("in main");
	std::string clientId = "1Ufd2DdgPNfSzlm7";
	std::string URL="speech.samespace.com:2612";
	//std::string URL = "localhost:8081";
	speechModule *ai_speech = new speechModule(clientId, URL);
	std::string audioBuff = ai_speech->speech_tts();
	printf("%s",audioBuff.c_str());
}
