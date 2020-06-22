#ifndef GRPC_CLIENT_H
#define GRPC_CLIENT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*    typedef struct samespacetts
    {
        char *voice;
        switch_memory_pool_t *pool;
        switch_buffer_t *audio_buffer;
    } samespacetts_t;

    typedef struct samespace
    {
        uint32_t flags;
        switch_mutex_t *flag_mutex;
        switch_memory_pool_t *pool;
        switch_buffer_t *text_buffer;
        switch_buffer_t *dtmf_buffer;
        switch_buffer_t *audio_buffer;
        size_t audio_size;
        char *rate;
        char *codec;
        int vad_flag;
        int listen_hits;
        int hangover_hits;
        int dtmf_hangover;
        int max_digits;
        int dtmf_only;
        int interruption;
	char* voiceName;
    } samespace_t;
    */

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
    //void textToSpeech(SAMESPACE_t *args, samespacetts_t *samespacetts);
    //void speechToText(samespace_t *samespace, char *data, int size);

#ifdef __cplusplus
}
#endif
#endif
