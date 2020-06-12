#include <cstdlib>
#include <switch.h>

#include <stdlib.h>
#include <string.h>
#include "speechInterface.h"
#include <switch_buffer.h>
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
        std::string speech_tts(SAMESPACE_t *p);
        std::string speech_sst(std::string data, char* voiceName);

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

Text MakeText(SAMESPACE_t *p)
{

        Text n;
        n.set_text(p->text);
        n.set_voicename(p->voiceName);
        return n;
}

void read(const std::string &filename, std::string &data)
{
        std::ifstream file(filename.c_str(), std::ios::in);

        if (file.is_open())
        {
                std::stringstream ss;
                ss << file.rdbuf();

                file.close();

                data = ss.str();
        }

        return;
}

speechModule::speechModule(std::string clientId, std::string ip_port)
{

        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Connection request to server [%s]",ip_port.c_str());
        std::string BearerToken = "Bearer " + clientId;
        auto call_creds = grpc::MetadataCredentialsFromPlugin(
            std::unique_ptr<grpc::MetadataCredentialsPlugin>(
                new MyCustomAuthenticator(BearerToken)));

        auto channel_credentials = grpc::SslCredentials(grpc::SslCredentialsOptions());
        auto credentials = grpc::CompositeChannelCredentials(channel_credentials, call_creds);
        stub_ = (TextToSpeech::NewStub(grpc::CreateChannel(ip_port, credentials)));
}

std::string speechModule::speech_tts(SAMESPACE_t *p)
{
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "API Test to Speech invoked!!!");
        Text request;
        Speech reply;

        request = MakeText(p);

        char *buff;
        ClientContext context;
        Status status = stub_->Say(&context, request, &reply);

        if (status.ok())
        {
                return reply.audio();
        }
        else
        {
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[Error code: message] -> [%d : %s]\n", status.error_code(), (status.error_message()).c_str());
                return "Failed";
        }
}

std::string speechModule::speech_sst(std::string data, char* voiceName)
{
        AsrSpeech req;
        Speech resp;

        ClientContext context;

        req.set_audio(data);
	req.set_model(voiceName);
        Status status = stub_->Asr(&context, req, &resp);

        if (status.ok())
        {
                return resp.audio();
        }
        else
        {
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[Error code: message] -> [%d : %s]\n", status.error_code(), (status.error_message()).c_str());
                return "Failed";
        }
}

#ifdef __cplusplus
extern "C"
{
#endif

        struct speechModule *ai_speech = NULL;
        int z = 0;
        void cppwrapperFuc(char *id, char* url)
        {
                std::string clientId(id);
		std::string URL(url);
                if (ai_speech == NULL)
                {
                        ai_speech = new speechModule(clientId, URL);
                }
        }

        void textToSpeech(SAMESPACE_t *args, samespacetts_t *samespacetts)
        {

                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "Api solution: textToSpeech is called \n");
                //SAMESPACE_t *myid = (SAMESPACE_t *)args;
                //std::string audioBuff = ttsc->Say((void *)myid); //Request to server
		std::string audioBuff = ai_speech->speech_tts(args);

                // audioBuff: server return audio data in bytes
                size_t n1 = audioBuff.size();
                size_t n2 = 1;
                int used = (int)switch_buffer_write(samespacetts->audio_buffer, &audioBuff[0], n1 * n2);
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "textToSpeech:->  \t Buffer length: [%zu]\t Actually written: [%d]", n1, used);
        }

        void speechToText(samespace_t *samespace, char *data, int size)
        {
                int i;
                string s = "";
                for (i = 0; i < size; i++)
                {
                        s = s + data[i];
                }

                std::string def("Failed");
                std::string text = ai_speech->speech_sst(s, samespace->voiceName);
                if (!text.empty() && text.compare(def))
                {
                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "\nSpecch to Text data: [%s] [%s]", text.c_str(), samespace->voiceName);
                        switch_buffer_write(samespace->text_buffer, text.c_str(), text.length());
                }
        }

#ifdef __cplusplus
}
#endif
