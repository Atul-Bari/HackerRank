/*
 * samespace Speech module
 *
 * sumeet <sumeet.tiwari@novanet.net>
 *
 * mod_samespace_asr.c - Samespace Interface
 *change
*/

#include <string.h>
#include <switch.h>
#include <stdbool.h>
#include <switch_cJSON.h>
#include <stdlib.h>
//#include <cstdlib>
#include <speechInterface.h>
//#include <aaa_c_connector.h>
#include <client.h>
#define PORT 2609
#include <stdio.h>
#include <sys/socket.h>

#define WAVE_FILE_LENGTH 126
#define WAVE_DIR_MAX_LENGTH 110

SWITCH_MODULE_LOAD_FUNCTION(mod_samespace_speech_load);
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_samespace_speech_shutdown);
SWITCH_MODULE_DEFINITION(mod_samespace_speech, mod_samespace_speech_load, mod_samespace_speech_shutdown, NULL);

static switch_mutex_t *MUTEX = NULL;
static switch_event_node_t *NODE = NULL;

//ASR Struct 
static struct {

    char* proxy_url;
    char* token;
    //ASR data
    int silence_avg_threshold;
    int silence_max_threshold;
    int feed_min_avg_energy;
    int feed_min_max_energy;
    int no_input_hangover;
    int silence_hangover;
    int min_listen_hits;
    //char *stt_websocket_url;
    //int stt_websocket_port;
    int connMonitor;

    //TTS data
    char *voice;
    char *audio_type;
    int buffer_size;
    int buffer_max_size;

    /* for audio and text buffer*/
    int audio_buffer_size;
    int audio_buffer_max_size;
    int text_buffer_size;
    int text_buffer_max_size;
    /* for debug */
    char *wav_file_dir;
	  int auto_reload;
    switch_memory_pool_t *pool;
} globals;


typedef enum {
	YY_FLAG_READY = (1 << 0),
	YY_FLAG_INPUT_TIMERS = (1 << 1),
	YY_FLAG_NOINPUT_TIMEOUT = (1 << 2),
	YY_FLAG_SPEECH_TIMEOUT = (1 << 3),
	YY_FLAG_RECOGNITION = (1 << 4),
	YY_FLAG_HAS_TEXT = (1 << 5),
} samespace_flag_t;


// TTS Starts HERE
static switch_status_t samespacetts_speech_open(
    switch_speech_handle_t *sh,
    const char *voice_name, int rate,
    int channels, switch_speech_flag_t *flags)
{
	 samespacetts_t *samespacetts = (samespacetts_t *)switch_core_alloc(sh->memory_pool, sizeof(samespacetts_t));

    if ( voice_name ) {
        samespacetts->voice = switch_core_strdup(sh->memory_pool, voice_name);
    } else {
        samespacetts->voice = globals.voice;
    }

    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Golbal contents:  %d \t %d\n", globals.buffer_size, globals.buffer_max_size);
    switch_buffer_create_dynamic(&samespacetts->audio_buffer,
        globals.buffer_size, globals.buffer_size, globals.buffer_max_size);

    samespacetts->pool = sh->memory_pool;
	  sh->private_info = samespacetts;

	  return SWITCH_STATUS_SUCCESS;
}

static switch_status_t samespacetts_speech_close(switch_speech_handle_t *sh, switch_speech_flag_t *flags)
{
	samespacetts_t *samespacetts = (samespacetts_t *) sh->private_info;

	if ( samespacetts->audio_buffer ) {
		switch_buffer_destroy(&samespacetts->audio_buffer);
	}

	return SWITCH_STATUS_SUCCESS;
}


static switch_status_t samespacetts_speech_feed_tts(switch_speech_handle_t *sh, char *text, switch_speech_flag_t *flags)
{

    	samespacetts_t *samespacetts = (samespacetts_t *) sh->private_info;

   	SAMESPACE_t *inputData = (SAMESPACE_t *)malloc(sizeof(SAMESPACE_t));

	char* arr[3];
	int i=0;
	char* p = strtok (text,"|");
  	while (p!= NULL)
  	{
    		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "Text to say: %s",p);
		arr[i++] = p;
    		p = strtok (NULL, "|");
  	}

   	stpcpy(inputData->text, arr[1]);
	stpcpy(inputData->voiceName, arr[0]);

    	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "Text to say: %s\t%s\t%s\t ", text, inputData->text, inputData->voiceName);
    	textToSpeech(inputData, samespacetts);

    	return SWITCH_STATUS_SUCCESS;
}

static void samespacetts_speech_flush_tts(switch_speech_handle_t *sh)
{
	samespacetts_t *samespacetts = (samespacetts_t *) sh->private_info;

	if ( samespacetts->audio_buffer ) {
	    switch_buffer_zero(samespacetts->audio_buffer);
	}
}

static switch_status_t samespacetts_speech_read_tts(switch_speech_handle_t *sh, void *data, size_t *datalen, switch_speech_flag_t *flags)
{
	size_t bytes_read;
	samespacetts_t *samespacetts = (samespacetts_t *) sh->private_info;
	
  switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "In buffer used: %lu\n", switch_buffer_inuse(samespacetts->audio_buffer));
    if ( ! samespacetts->audio_buffer
          || switch_buffer_inuse(samespacetts->audio_buffer) == 0 ) {
	     switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "FALSE Return: ");
          return SWITCH_STATUS_FALSE;
	  }

	if ( (bytes_read = switch_buffer_read(samespacetts->audio_buffer, data, *datalen)) ) {
		*datalen = bytes_read;
		return SWITCH_STATUS_SUCCESS;
	}

	//*datalen = my_datalen * 2;
	return SWITCH_STATUS_FALSE;
}
//uuid_transfer <uuid> 'm:^:play_and_detect_speech:say:How are you? detect:samespace_asr {ip=10.24.48.173,port=2609,max_digits=1,dtmf_only=1,interruption=1}grammar^park' inline samespace
static void samespacetts_text_param_tts(switch_speech_handle_t *sh, char *param, const char *val)
{

    /*char* ip = malloc(16);
    char* port = malloc(10);
    char* maxd = malloc(15);
    char* onlydtmf = malloc(2);
    char* interruption = malloc(2);*/
	/*char* text = malloc(50);
	samespacetts_t* samespacetts = (samespace_t *) sh->private_info;
  	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "tts_text_param %s %s\n", param, val);
  	if(strcmp(param, "text") == 0){
    	strcpy(text, val);
    	samespacetts->voice = text;
  }*/


}

static void samespacetts_numeric_param_tts(switch_speech_handle_t *sh, char *param, int val)
{
}

static void samespacetts_float_param_tts(switch_speech_handle_t *sh, char *param, double val)
{
}
//TTS End here



//ASR starts here
/**
 * get the switch buffer ptr which is safe for std string operation
*/
static char *get_switch_buffer_ptr(switch_buffer_t *buffer)
{
    	char *ptr = (char*)switch_buffer_get_head_pointer(buffer);
    	ptr[switch_buffer_inuse(buffer)] = '\0';
    	return ptr;
}

static switch_bool_t recognize_handler(samespace_t *samespace){
 	speechToText(samespace, get_switch_buffer_ptr(samespace->audio_buffer), switch_buffer_inuse(samespace->audio_buffer));  
 	switch_buffer_zero(samespace->audio_buffer);
  	return SWITCH_TRUE;
}

static switch_bool_t recognize_end(samespace_t *samespace){ 
  	speechToText(samespace, "END", 3);
  	return SWITCH_TRUE;
}
static switch_bool_t recognize_close(samespace_t *samespace){
  	speechToText(samespace, "END", 3);
  	return SWITCH_TRUE;
}

/**
 * internal function to recv the audio input and do the stop detect checking
 *
 * @param   ah
 * @param   data
 * @param   samples
*/

static switch_bool_t stop_audio_detect(
    switch_asr_handle_t *ah, int16_t *data, unsigned int samples)
{
    register int16_t abs_sample;
    double energy = 0;
    uint32_t c = 0, avg_energy, max_energy;
    samespace_t *samespace = (samespace_t *) ah->private_info;

    abs_sample = abs(data[0]);
    energy = abs_sample;
    max_energy = abs_sample;
    for ( c = 1; c < samples; c++ ) {
        abs_sample = abs(data[c]);
        energy += abs_sample;
        if ( abs_sample > max_energy ) {
            max_energy = abs_sample;
        }
    }

    avg_energy = (uint32_t) (energy / samples);
    if(samespace->interruption == 1){
      if ( avg_energy > globals.silence_avg_threshold + 550
          || max_energy > globals.silence_max_threshold + 900) {
          samespace->hangover_hits = 0;
          samespace->listen_hits++;
      } else if(samespace->listen_hits > globals.min_listen_hits){
          samespace->hangover_hits++;
      }
    }else{
      if ( avg_energy > globals.silence_avg_threshold
          || max_energy > globals.silence_max_threshold ) {
          samespace->hangover_hits = 0;
          samespace->listen_hits++;
      } else{
          samespace->hangover_hits++;
      }
    }

    /* copying the sample data to the audio buffer
     * for recognition usage later (2 bytes for each sample) */
    switch_log_printf(
        SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO,
        "hangover_hits=%d, listen_hits=%d, avg_energy=%u, max_energy=%u samples=%u buffer_length=%lu vad_flag=%d dtmf_hangover=%d dtmf_only=%d dtmf_buffer=%lu\n",
        samespace->hangover_hits, samespace->listen_hits, avg_energy, max_energy, samples, switch_buffer_inuse(samespace->audio_buffer), samespace->vad_flag, samespace->dtmf_hangover, samespace->max_digits, switch_buffer_inuse(samespace->dtmf_buffer)
    );
    if(samespace->dtmf_only == 0){
      if ( (avg_energy > globals.feed_min_avg_energy
          || max_energy > globals.feed_min_max_energy) && switch_buffer_inuse(samespace->dtmf_buffer) == 0) {
          /* resample the data */
          switch_buffer_write(samespace->audio_buffer, data, samples * sizeof(int16_t));
          samespace->audio_size += samples * sizeof(int16_t);
          if ( samespace->audio_size >= globals.audio_buffer_max_size ) {
              samespace->audio_size = globals.audio_buffer_max_size;
              return SWITCH_TRUE;
          }
      }
      if(samespace->vad_flag == 0){
        recognize_handler(samespace);
      }

      if(samespace->hangover_hits >= globals.silence_hangover && samespace->vad_flag == 0 && samespace->listen_hits > globals.min_listen_hits){
        samespace->vad_flag = 1;
        recognize_end(samespace);
      }

      if(switch_buffer_inuse(samespace->dtmf_buffer) > 0){
        if(samespace->interruption == 0){
          samespace->dtmf_hangover++;
        }
        if(samespace->vad_flag == 0){
          samespace->vad_flag = 1;
          recognize_end(samespace);
        }
      }
      if(switch_buffer_inuse(samespace->dtmf_buffer) >= samespace->max_digits || samespace->dtmf_hangover > 200){
        samespace->audio_size = 1;
        samespace->vad_flag = 0;
        samespace->dtmf_hangover = 0;
        switch_mutex_lock(samespace->flag_mutex);
        switch_clear_flag(samespace, YY_FLAG_READY);
        switch_set_flag(samespace, YY_FLAG_SPEECH_TIMEOUT);
        switch_mutex_unlock(samespace->flag_mutex);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "got dtmf timeout\n");
        return SWITCH_TRUE;
      }
      if(switch_buffer_inuse(samespace->dtmf_buffer) > 0){
        return SWITCH_FALSE;
      }
      if ( samespace->hangover_hits > 0 && switch_test_flag(samespace, YY_FLAG_INPUT_TIMERS) ) {
          if ( samespace->listen_hits <= globals.min_listen_hits ) {
              if ( samespace->hangover_hits >= globals.no_input_hangover
                  && ! switch_test_flag(samespace, YY_FLAG_NOINPUT_TIMEOUT) ) {
                  /*no input timeout*/
                  samespace->vad_flag = 0;
                  switch_mutex_lock(samespace->flag_mutex);
                  switch_clear_flag(samespace, YY_FLAG_READY);
                  switch_set_flag(samespace, YY_FLAG_NOINPUT_TIMEOUT);
                  switch_mutex_unlock(samespace->flag_mutex);
                  switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "no input timeout\n");
                  return SWITCH_TRUE;
              }
          } else if ( switch_buffer_inuse(samespace->text_buffer)
              && ! switch_test_flag(samespace, YY_FLAG_SPEECH_TIMEOUT) ) {
              /*silence timeout*/
              samespace->vad_flag = 0;
              switch_mutex_lock(samespace->flag_mutex);
              switch_clear_flag(samespace, YY_FLAG_READY);
              switch_set_flag(samespace, YY_FLAG_SPEECH_TIMEOUT);
              switch_mutex_unlock(samespace->flag_mutex);
              switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "got text timeout\n");
              return SWITCH_TRUE;
          } else if ( samespace->hangover_hits >= globals.silence_hangover + 20
              && ! switch_test_flag(samespace, YY_FLAG_SPEECH_TIMEOUT) ) {
              /*silence timeout*/
              samespace->vad_flag = 0;
              switch_mutex_lock(samespace->flag_mutex);
              switch_clear_flag(samespace, YY_FLAG_READY);
              switch_set_flag(samespace, YY_FLAG_SPEECH_TIMEOUT);
              switch_mutex_unlock(samespace->flag_mutex);
              switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "silence timeout\n");
              return SWITCH_TRUE;
          }
      }
    }else{
      samespace->vad_flag = 1;
      if(samespace->interruption == 0){
        samespace->dtmf_hangover++;
      }
      if(switch_buffer_inuse(samespace->dtmf_buffer) >= samespace->max_digits || samespace->dtmf_hangover > 200){
        samespace->audio_size = 1;
        samespace->vad_flag = 0;
        samespace->dtmf_hangover = 0;
        switch_mutex_lock(samespace->flag_mutex);
        switch_clear_flag(samespace, YY_FLAG_READY);
        switch_set_flag(samespace, YY_FLAG_SPEECH_TIMEOUT);
        switch_mutex_unlock(samespace->flag_mutex);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "got dtmf timeout\n");
        return SWITCH_TRUE;
      }
    }
    /* Check the silence timeout */

    return SWITCH_FALSE;
}

/*
 * function to open the asr interface
 * invoke once when start the asr engine
*/

static switch_status_t samespace_asr_open(
    switch_asr_handle_t *ah,
    const char *codec, int rate,
    const char *dest, switch_asr_flag_t *flags)
{
    samespace_t *samespace =(samespace_t *)switch_core_alloc(ah->memory_pool, sizeof(samespace_t));
    if ( samespace == NULL ) {
        return SWITCH_STATUS_MEMERR;
    }
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "GETTING RATE FROM FS %d\n", rate);
  	ah->codec = switch_core_strdup(ah->memory_pool, codec);

    if (rate == 8000) {
  		ah->rate = 8000;
  	} else {
      switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Invalid rate %d. Only 8000 is supported.\n", rate);
    }

    ah->private_info = samespace;
    samespace->flags = 0;
    samespace->pool = ah->memory_pool;
    samespace->codec = ah->codec;
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "asr_handle: name = %s, codec = %s, rate = %d, grammar = %s, param = %s, dest = %s\n",
  					  ah->name, ah->codec, ah->rate, ah->grammar, ah->param, dest);
    switch_mutex_init(&samespace->flag_mutex, SWITCH_MUTEX_NESTED, ah->memory_pool);
    samespace->audio_size = 0;

    // initialized the audio_buffer and the text_buffer
    switch_buffer_create_dynamic(&samespace->audio_buffer,
        globals.audio_buffer_size,
        globals.audio_buffer_size, globals.audio_buffer_max_size);
    switch_buffer_create_dynamic(&samespace->text_buffer,
        globals.text_buffer_size,
        globals.text_buffer_size, globals.text_buffer_max_size);

    switch_buffer_create_dynamic(&samespace->dtmf_buffer,
        globals.text_buffer_size,
        globals.text_buffer_size, globals.text_buffer_max_size);

    samespace->hangover_hits = 0;
    samespace->listen_hits = 0;
    samespace->vad_flag = 0;
    samespace->max_digits = 1;
    samespace->dtmf_only = 0;
    samespace->interruption = 0;
    samespace->dtmf_hangover = 0;
    samespace->voiceName = 0;// 0 is for default KALDI 
    switch_mutex_lock(samespace->flag_mutex);
    switch_set_flag(samespace, YY_FLAG_READY);
    switch_set_flag(samespace, YY_FLAG_INPUT_TIMERS);
    switch_mutex_unlock(samespace->flag_mutex);

	  return SWITCH_STATUS_SUCCESS;
}

/*! function to close the asr interface */
static switch_status_t samespace_asr_close(
    switch_asr_handle_t *ah, switch_asr_flag_t *flags)
{
    samespace_t *samespace = (samespace_t *) ah->private_info;

    recognize_close(samespace);

    if ( samespace->audio_buffer ) {
        switch_buffer_destroy(&samespace->audio_buffer);
    }

    if ( samespace->text_buffer ) {
        switch_buffer_destroy(&samespace->text_buffer);
    }

    if ( samespace->dtmf_buffer ) {
        switch_buffer_destroy(&samespace->dtmf_buffer);
    }

	switch_clear_flag(samespace, YY_FLAG_READY);
	switch_set_flag(ah, SWITCH_ASR_FLAG_CLOSED);
	return SWITCH_STATUS_SUCCESS;
}

/*! function to feed audio to the ASR */
static switch_status_t samespace_asr_feed(
    switch_asr_handle_t *ah, void *data, unsigned int len, switch_asr_flag_t *flags)
{
    time_t time_ptr;
    int timestamp;
    char wave_file[WAVE_FILE_LENGTH];
    FILE *stream = NULL;

    samespace_t *samespace = (samespace_t *) ah->private_info;

    // check the asr close flag
    if ( switch_test_flag(ah, SWITCH_ASR_FLAG_CLOSED) ) {
        return SWITCH_STATUS_BREAK;
    }
    if ( switch_test_flag(samespace, YY_FLAG_READY)
            && ! switch_test_flag(samespace, YY_FLAG_RECOGNITION) ) {
        if ( stop_audio_detect(ah, (int16_t *) data, len / 2)
            && samespace->audio_size > 0
                && ! switch_test_flag(samespace, YY_FLAG_NOINPUT_TIMEOUT) ) {
            switch_set_flag_locked(samespace, YY_FLAG_RECOGNITION);

            if ( globals.wav_file_dir != NULL && strlen(globals.wav_file_dir) < WAVE_DIR_MAX_LENGTH ) {
                timestamp = time(&time_ptr);
                memset(wave_file, 0x00, sizeof(wave_file));
                sprintf(wave_file, "%s/%d.pcm", globals.wav_file_dir, timestamp);
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "wave_file=%s\n", wave_file);
                stream = fopen(wave_file, "wb");
                if ( stream != NULL ) {
                    fwrite(
                        get_switch_buffer_ptr(samespace->audio_buffer),
                        switch_buffer_inuse(samespace->audio_buffer), 1, stream
                    );
                    fclose(stream);
                }
            }

            switch_mutex_lock(samespace->flag_mutex);
            switch_clear_flag(samespace, YY_FLAG_RECOGNITION);
            switch_set_flag(samespace, YY_FLAG_HAS_TEXT);
            switch_mutex_unlock(samespace->flag_mutex);
        } else if ( switch_test_flag(samespace, YY_FLAG_NOINPUT_TIMEOUT) ) {
            /* never heard anything */
            switch_buffer_zero(samespace->dtmf_buffer);
            switch_mutex_lock(samespace->flag_mutex);
            switch_set_flag(samespace, YY_FLAG_HAS_TEXT);
            switch_clear_flag(samespace, YY_FLAG_NOINPUT_TIMEOUT);
            switch_clear_flag(samespace, YY_FLAG_READY);
            switch_mutex_unlock(samespace->flag_mutex);
        }
    }

	  return SWITCH_STATUS_SUCCESS;
}

/*! function to pause recognizer */
static switch_status_t samespace_asr_pause(switch_asr_handle_t *ah)
{
  samespace_t *samespace = (samespace_t *) ah->private_info;
	switch_status_t status = SWITCH_STATUS_FALSE;

	switch_mutex_lock(samespace->flag_mutex);
	if ( switch_test_flag(samespace, YY_FLAG_READY) ) {
		switch_clear_flag(samespace, YY_FLAG_READY);
		status = SWITCH_STATUS_SUCCESS;
	}
	switch_mutex_unlock(samespace->flag_mutex);
  return status;
}


static switch_status_t samespace_asr_feed_dtmf(switch_asr_handle_t *ah, const switch_dtmf_t *dtmf, switch_asr_flag_t *flags)
{
  samespace_t *samespace = (samespace_t *) ah->private_info;
  char digits[1];
  digits[0] = dtmf->digit;
  switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO,
      "DTMF : %d\n", dtmf->digit);
  /*Press Pound key for terminating dtmf*/
  switch_buffer_write(samespace->dtmf_buffer, digits, sizeof(digits));
  samespace->dtmf_hangover = 0;
  return SWITCH_STATUS_SUCCESS;
}

/*! function to resume recognizer */
static switch_status_t samespace_asr_resume(switch_asr_handle_t *ah)
{
  samespace_t *samespace = (samespace_t *) ah->private_info;
	switch_status_t status = SWITCH_STATUS_FALSE;

	switch_mutex_lock(samespace->flag_mutex);
	if ( ! switch_test_flag(samespace, YY_FLAG_READY) ) {
        /*zero fill the audio_buffer and the audio size*/
        switch_buffer_zero(samespace->audio_buffer);
        switch_buffer_zero(samespace->text_buffer);
        switch_buffer_zero(samespace->dtmf_buffer);
        samespace->audio_size = 0;
        samespace->hangover_hits = 0;
        samespace->listen_hits = 0;

        /*clear all the to stop flag*/
		    switch_set_flag(samespace, YY_FLAG_READY);
        switch_clear_flag(samespace, YY_FLAG_SPEECH_TIMEOUT);
        switch_clear_flag(samespace, YY_FLAG_NOINPUT_TIMEOUT);

		status = SWITCH_STATUS_SUCCESS;
	}
	switch_mutex_unlock(samespace->flag_mutex);

  return status;
}

/*! function to read results from the ASR*/
static switch_status_t samespace_asr_check_results(
    switch_asr_handle_t *ah, switch_asr_flag_t *flags)
{
    samespace_t *samespace = (samespace_t *) ah->private_info;
    return switch_test_flag(samespace, YY_FLAG_HAS_TEXT)
        ? SWITCH_STATUS_SUCCESS : SWITCH_STATUS_FALSE;
}

/*! function to read results from the ASR */
static switch_status_t samespace_asr_get_results(switch_asr_handle_t *ah, char **xmlstr, switch_asr_flag_t *flags)
{
    samespace_t *samespace = (samespace_t *) ah->private_info;

    if ( switch_test_flag(samespace, YY_FLAG_HAS_TEXT) ) {
        if(switch_buffer_inuse(samespace->dtmf_buffer) > 0){
          *xmlstr = switch_mprintf("%s <DTMF>:%s",get_switch_buffer_ptr(samespace->text_buffer), get_switch_buffer_ptr(samespace->dtmf_buffer));
        }else{
          *xmlstr = switch_mprintf("%s",get_switch_buffer_ptr(samespace->text_buffer));
        }
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR,
            "get_results.dtmf=%s\n", get_switch_buffer_ptr(samespace->dtmf_buffer));
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR,
            "get_results.text=%s\n", *xmlstr);
        switch_clear_flag_locked(samespace, YY_FLAG_HAS_TEXT);

        return SWITCH_STATUS_SUCCESS;
    }
    return SWITCH_STATUS_FALSE;
}

/*! function to start input timeouts */
static switch_status_t samespace_asr_start_input_timers(switch_asr_handle_t *ah)
{
	samespace_t *samespace = (samespace_t *) ah->private_info;
	switch_set_flag_locked(samespace, YY_FLAG_INPUT_TIMERS);
  samespace->interruption = 0;
  switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "start_input_timers\n");
	return SWITCH_STATUS_SUCCESS;
}


/*! function to load a grammar to the asr interface */
static switch_status_t samespace_asr_load_grammar(switch_asr_handle_t *ah, const char *grammar, const char *name)
{
  // char *ip = malloc(16);
  // char *port = malloc(10);
  // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "LOADING GRAMMAR NOW\n");
  // if(ip != NULL)
  //   strcpy(ip, grammar);
  // globals.stt_websocket_url = ip;
  // if ((name != NULL) && (name[0] == '\0')) {
  //   switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "name is null\n");
  // }else{
  //   if(port != NULL)
  //     strcpy(port, name);
  //   switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "loading grammar as config %s %s\n", grammar, name);
  //   globals.stt_websocket_port = atoi(port);
  // }
  return SWITCH_STATUS_SUCCESS;
}

/*! function to unload a grammar to the asr interface */
static switch_status_t samespace_asr_unload_grammar(switch_asr_handle_t *ah, const char *name)
{
	return SWITCH_STATUS_SUCCESS;
}

/*! set text parameter */
static void samespace_asr_text_param(switch_asr_handle_t *ah, char *param, const char *val)
{
  char *maxd =(char*) malloc(15);
  char *onlydtmf = (char*)malloc(2);
  char *interruption = (char*)malloc(2);
  char *voiceName = (char *) malloc(30 * sizeof(char));
  samespace_t *samespace = (samespace_t *) ah->private_info;
  switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "asr_text_param %s %s\n", param, val);
  if(strcmp(param, "max_digits") == 0){
    	strcpy(maxd, val);
    	samespace->max_digits = atoi(maxd);
  }else if(strcmp(param, "dtmf_only") == 0){
    	strcpy(onlydtmf, val);
    	samespace->dtmf_only = atoi(onlydtmf);
  }else if(strcmp(param, "interruption") == 0){
    	strcpy(interruption, val);
    	samespace->interruption = atoi(interruption);
  }else if(strcmp(param, "model") == 0) {
	strcpy(voiceName, val);
	samespace->voiceName = voiceName;
  }
	
  /* }else if(strcmp(param, "model") == 0){
	strcpy(voiceName, val);
    	if(!strcmp(voiceName, "en-US-Generic")) {
		samespace->voiceName = 1;
	} else if(!strcmp(voiceName, "en-US-HC")) {
		samespace->voiceName = 2;
	} else if(!strcmp(voiceName, "en-IN-Generic")) {
		samespace->voiceName = 3;
	} else {
		samespace->voiceName = 1;
	}
  }*/
}

/*! set numeric parameter */
static void samespace_asr_numeric_param(switch_asr_handle_t *ah, char *param, int val)
{
}

/*! set float parameter */
static void samespace_asr_float_param(switch_asr_handle_t *ah, char *param, double val)
{
}

static switch_status_t samespace_load_config(void)
{
	switch_status_t status = SWITCH_STATUS_SUCCESS;

	globals.silence_avg_threshold = 300;
  	globals.silence_max_threshold = 540;
  	globals.feed_min_avg_energy = 30;
  	globals.feed_min_max_energy = 150;
  	globals.no_input_hangover = 500;
  	globals.silence_hangover = 30;
  	globals.min_listen_hits = 8;
	globals.audio_buffer_size = 1024 * 64;
	globals.audio_buffer_max_size = 1024 * 1024 * 4;
	globals.text_buffer_size = 96;
	globals.text_buffer_max_size = 1024 * 64;
  	globals.wav_file_dir = NULL;
  	globals.connMonitor = 0;
	return status;
}


static switch_status_t samespace_tts_load_config(void)
{
    char *cf = "mod_samespace_speech.conf";
   //char *cf = "modules.conf";
    switch_xml_t cfg, xml = NULL, param, settings;
	  switch_status_t status = SWITCH_STATUS_SUCCESS;

    globals.voice = "zhilingf";	
    globals.audio_type = "wav";
    globals.buffer_size = 49152;
    globals.buffer_max_size = 4194304;
    globals.auto_reload = 1;
    globals.proxy_url = "speech.samespace.com:8080";

    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "In xx xttults_load_config\n\n");
    if ( ! (xml = switch_xml_open_cfg(cf, &cfg, NULL)) ) {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Open of %s failed\n", cf);
    		status = SWITCH_STATUS_FALSE;
    		goto done;
    }

    if ( (settings = switch_xml_child(cfg, "settings")) ) {
		for ( param = switch_xml_child(settings, "param"); param; param = param->next ) {
			char *var = (char *) switch_xml_attr_soft(param, "name");
			char *val = (char *) switch_xml_attr_soft(param, "value");
 /*     if ( strcasecmp(var, "audio-type") == 0 ) {
				globals.audio_type = switch_core_strdup(globals.pool, val);
			} else if ( strcasecmp(var, "buffer-size") == 0 ) {
				globals.buffer_size = atoi(val);
			} else if ( strcasecmp(var, "buffer-max-size") == 0 ) {
				globals.buffer_max_size = atoi(val);
			} else if ( strcasecmp(var, "audo-reload") == 0 ) {
				globals.auto_reload = switch_true(val);
			} else*/
		       	if ( strcasecmp(var, "proxy-url") == 0 ) {
                                globals.proxy_url = val;
                        } else if ( strcasecmp(var, "token") == 0 ) {
                                globals.token = val;
                        }
		}
	}
    done:
      if (xml) {
		    switch_xml_free(xml);
	    }

    return status;
}


static void do_config_load(void)
{
	switch_mutex_lock(MUTEX);
	samespace_load_config();
  samespace_tts_load_config();
	switch_mutex_unlock(MUTEX);
}

static void event_handler(switch_event_t *event)
{
	if ( globals.auto_reload ) {
		do_config_load();
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "samespace reloaded now in new push\n");
	}
}


SWITCH_MODULE_LOAD_FUNCTION(mod_samespace_speech_load)
{
	switch_asr_interface_t *asr_interface = NULL;
  switch_speech_interface_t *tts_interface = NULL;

  switch_mutex_init(&MUTEX, SWITCH_MUTEX_NESTED, pool);
  globals.pool = pool;

  switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "In Main LOAD_FUNC\n\n");
	if ((switch_event_bind_removable(modname, SWITCH_EVENT_RELOADXML, NULL, event_handler, NULL, &NODE) != SWITCH_STATUS_SUCCESS)) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Couldn't bind!\n");
	}

	do_config_load();

	/* connect my internal structure to the blank pointer passed to me */
  *module_interface = switch_loadable_module_create_module_interface(pool, modname);

  tts_interface  = (switch_speech_interface_t*)switch_loadable_module_create_interface(*module_interface, SWITCH_SPEECH_INTERFACE);
	tts_interface->interface_name = "samespace_tts";
	tts_interface->speech_open = samespacetts_speech_open;
	tts_interface->speech_close = samespacetts_speech_close;
	tts_interface->speech_feed_tts = samespacetts_speech_feed_tts;
	tts_interface->speech_read_tts = samespacetts_speech_read_tts;
	tts_interface->speech_flush_tts = samespacetts_speech_flush_tts;
	tts_interface->speech_text_param_tts = samespacetts_text_param_tts;
	tts_interface->speech_numeric_param_tts = samespacetts_numeric_param_tts;
	tts_interface->speech_float_param_tts = samespacetts_float_param_tts;

	//module_interface = switch_loadable_module_create_module_interface(pool, modname);
	asr_interface = (switch_asr_interface_t*)switch_loadable_module_create_interface(*module_interface, SWITCH_ASR_INTERFACE);
	asr_interface->interface_name = "samespace_asr";
	asr_interface->asr_open = samespace_asr_open;
	asr_interface->asr_load_grammar = samespace_asr_load_grammar;
	asr_interface->asr_unload_grammar = samespace_asr_unload_grammar;
	asr_interface->asr_close = samespace_asr_close;
	asr_interface->asr_feed = samespace_asr_feed;
	asr_interface->asr_resume = samespace_asr_resume;
	asr_interface->asr_pause = samespace_asr_pause;
	asr_interface->asr_feed_dtmf = samespace_asr_feed_dtmf;
	asr_interface->asr_check_results = samespace_asr_check_results;
	asr_interface->asr_get_results = samespace_asr_get_results;
	asr_interface->asr_start_input_timers = samespace_asr_start_input_timers;
	asr_interface->asr_text_param = samespace_asr_text_param;
	asr_interface->asr_numeric_param = samespace_asr_numeric_param;
	asr_interface->asr_float_param = samespace_asr_float_param;


	/* indicate that the module should continue to be loaded */
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "In Main LOAD_FUNC\n\n");
	cppwrapperFuc("test","speech.samespace.com:8080");
//	AAA_sayHi("Atul this is test");
//    AAA_sayHi("James");

	return SWITCH_STATUS_SUCCESS;
}

SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_samespace_speech_shutdown)
{
  switch_event_unbind(&NODE);
	return SWITCH_STATUS_UNLOAD;
}

/* For Emacs:
 * Local Variables:
 * mode:c
 * indent-tabs-mode:t
 * tab-width:4
 * c-basic-offset:4
 * End:
 * For VIM:
 * vim:set softtabstop=4 shiftwidth=4 tabstop=4 noet:
 */
