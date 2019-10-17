/* Aduio Output Source File*/

#include "wavesynth.h"
#include "audio.h"
#include <alsa/asoundlib.h>
#include <stdio.h>

snd_pcm_t *playback_handle; 
snd_pcm_hw_params_t *hw_params; 
snd_pcm_sw_params_t *sw_params; 
snd_pcm_sframes_t frames_to_deliver;

unsigned int rate = 44100;
extern int dds_flag;
extern short samples[TOTAL_NUMBER_FREQUENCIES];
int playback_callback (snd_pcm_sframes_t nframes) {
    int err;
    /* ... fill buffer with data ... */
    dds_flag = 1;
    if ((err = snd_pcm_writei (playback_handle, samples, nframes)) < 0) {
        fprintf (stderr, "write failed (%s)\n", snd_strerror (err));
    }
    return err;
}

void start_audio_playback() {
    snd_pcm_start(playback_handle);
}
void init_hardware_parameters (){
    int err;
	/*INITIALIZE AUDIO*/
    snd_pcm_hw_params_t *hw_params;
    if ((err = snd_pcm_open (&playback_handle, "default" , SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf (stderr, "cannot open audio device %s (%s)\n",
                 "default",
                 snd_strerror (err));
        exit (1);
    }
    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
                 snd_strerror (err));
        exit (1);
    }
    if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
                 snd_strerror (err));
        exit (1);
    }
    if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf (stderr, "cannot set access type (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
        fprintf (stderr, "cannot set sample format (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &rate, 0)) < 0) {
        fprintf (stderr, "cannot set sample rate (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    if((err = snd_pcm_hw_params_set_periods(playback_handle, hw_params, 4,0)) < 0) {
        fprintf(stderr, "Cannot set period size (%s)\n", snd_strerror (err));
        exit (1);
    }
    int channels = 1;// Example
    if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, channels)) < 0) {
        fprintf (stderr, "cannot set channel count (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    snd_pcm_uframes_t frames = 128;// Example  
    if((err = snd_pcm_hw_params_set_buffer_size_near(playback_handle, hw_params, &frames)) < 0) {
    	fprintf(stderr, "Cannot set sample buffer size (%s)\n", snd_strerror (err));
    	exit (1);
    }
    snd_pcm_uframes_t actualSize;
    snd_pcm_hw_params_get_buffer_size(hw_params, &actualSize);
    fprintf(stderr, "SIZE: %i", (int) actualSize);

    if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot set parameters (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    snd_pcm_hw_params_free (hw_params);


}

void init_software_parameters (){
    int err;
if ((err = snd_pcm_sw_params_malloc (&sw_params)) < 0) {
        fprintf (stderr, "cannot allocate software parameters structure (%s)\n",
                 snd_strerror (err));
        exit (1);
    }
    if ((err = snd_pcm_sw_params_current (playback_handle, sw_params)) < 0) {
        fprintf (stderr, "cannot initialize software parameters structure (%s)\n",
                 snd_strerror (err));
        exit (1);
    }
    if ((err = snd_pcm_sw_params_set_avail_min (playback_handle, sw_params, PERIOD_SAMPLES)) < 0) {
        fprintf (stderr, "cannot set minimum available count (%s)\n",
                 snd_strerror (err));
        exit (1);
    }
    if ((err = snd_pcm_sw_params_set_start_threshold (playback_handle, sw_params, PERIOD_SAMPLES)) < 0) {
        fprintf (stderr, "cannot set start mode (%s)\n",
                 snd_strerror (err));
        exit (1);
    }
    if ((err = snd_pcm_sw_params (playback_handle, sw_params)) < 0) {
        fprintf (stderr, "cannot set software parameters (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    /* the interface will interrupt the kernel every 4096 frames, and ALSA
       will wake up this program very soon after that.
    */

    if ((err = snd_pcm_prepare (playback_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

}

void process_callback (){
	/* Wait for room in buffer using polling */
    int err;

	if ((err = snd_pcm_wait (playback_handle, 1)) < 0) {
		fprintf (stderr, "poll failed (%s)\n", strerror (errno));
		//break;
	}

	/* Find out how much space is available for playback data */

	if ((frames_to_deliver = snd_pcm_avail_update (playback_handle)) < 0) {
		if (frames_to_deliver == -EPIPE) {
			fprintf (stderr, "an xrun occured\n");
			//break;
		} else {
			fprintf (stderr, "unknown ALSA avail update return value (%d)\n",
					 (int) frames_to_deliver);
			//break;
		}
	}
	/* Empty space grater than buffer size ? */
	if(frames_to_deliver > PERIOD_SAMPLES) {
		if (playback_callback (PERIOD_SAMPLES) != PERIOD_SAMPLES) {
			fprintf (stderr, "playback callback failed\n");
			//break;
		}

	}
}
