#ifndef SYNTH_USB
#define SYNTH_USB

#include <stdio.h>
#include <alsa/asoundlib.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "wavesynth.h"
#include <poll.h>
#include <errno.h>
#include <math.h>

extern int tuning_lookup[];

extern int note_buffer[120];
extern int note_buffer_ptr;
extern int config_buffer[4];
extern int config_buffer_ptr;


void *usb(void* );

#endif 

