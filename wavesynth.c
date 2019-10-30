#include <stdio.h>
#include <alsa/asoundlib.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include "wavesynth.h"
#include "dds.h"
#include "USB.h"
#include "audio.h"
#include "config/config.h"

int main(int argc, char *argv[])
{

    //Global variables
    short *wavetable;
    int numberInstruments;
    instrument *instrumentArray;
    instrument currentInstrument;
    DDS_data DDS_data_array[TOTAL_NUMBER_FREQUENCIES]; //DDS information buffer
    USB_data USB_data_array[TOTAL_NUMBER_NOTES];       //USB information buffer

    init_wavetable(&wavetable);
    createInstrumentArray(&numberInstruments, &instrumentArray);

    init_DDS(samples);

    init_hardware_parameters();
    init_software_parameters();

    //DDS information buffer setup
    for (int i = 0; i < TOTAL_NUMBER_FREQUENCIES; i++)
    {
        DDS_data_array[i].tuning_word = 0;
        DDS_data_array[i].attenuate = 0;
        DDS_data_array[i].enable = 0;
    }
    //USB note information buffer setup
    for (int i = 0; i < TOTAL_NUMBER_NOTES; i++)
    {
        USB_data_array[i].midi_note = -1;
        USB_data_array[i].attenuation_vector = 0;
        USB_data_array[i].age = 0;
        USB_data_array[i].state = off;
    }

    currentInstrument = instrumentArray[0];
    pthread_t thread_id;
    USB_thread_args USB_args;
    USB_args.data = DDS_data_array;
    USB_args.usb_data_array = USB_data_array;
    USB_args.instrumentArray = &instrumentArray;
    USB_args.currentInstrument = &currentInstrument;
    USB_args.numberInstruments = numberInstruments;
    pthread_create(&thread_id, NULL, usb, &USB_args);
    //start_audio_playback();
    while (1)
    {
        if (dds_flag)
        {
            dds_flag = 0;
            DDS(DDS_data_array, USB_data_array, currentInstrument.numHarmonics, wavetable, samples);
        }
        process_callback();
    }
    return 0;
}
