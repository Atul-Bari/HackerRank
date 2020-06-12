#ifndef GRPC_CLIENT_H
#define GRPC_CLIENT_H

#ifdef __cplusplus
extern "C"
{
#endif


    typedef struct ttsInput
    {
        char text[500];
        char voiceName[100];
    } TTSINPUT_t;

    typedef struct samespace1
    {
        char text[500];
        char 	voiceName[100];

    } SAMESPACE_t;

    void cppwrapperFuc(char *id, char* url);
//    void textToSpeech(SAMESPACE_t *args, samespacetts_t *samespacetts);
//    void speechToText(samespace_t *samespace, char *data, int size);

#ifdef __cplusplus
}
#endif
#endif
