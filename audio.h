#ifndef AUDIO_FILE
#define AUDIO_FILE
/* Audio Output Header File*/

#include <alsa/asoundlib.h>

// Declaration of Functions
int playback_callback(snd_pcm_sframes_t);
void init_hardware_parameters();
void init_software_parameters();
void process_callback();
void start_audio_playback();

#endif