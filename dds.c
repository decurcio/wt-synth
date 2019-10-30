#include "wavesynth.h"
#include <stdio.h>
int accumulators[TOTAL_NUMBER_FREQUENCIES];
void init_DDS(short *samples)
{
    //Initialization function for DDS
    for (int i = 0; i < TOTAL_NUMBER_FREQUENCIES; i++)
    {
        accumulators[i] = 0;
        samples[i] = 0;
    }
}
float ATTENUATE_FACTOR = 0.125;
void DDS(void *DDS_data_array, void *USB_data_array, int numHarmonics, short *wavetable, short *samples)
{
    DDS_data *dds_data = DDS_data_array;
    USB_data *usb_data = USB_data_array;

    //attenuation cutoffs; will be determined by instrument attenuation value in the future.
    int A_cutoff = 4000;
    int D_cutoff = 48100;
    int S_cutoff = D_cutoff;
    int R_cutoff = D_cutoff + 44100;

    //attenuation calculation values
    float A_lerp_mult = 1.0 / A_cutoff;
    float D_lerp_mult = 1.0 / (D_cutoff - A_cutoff);
    float R_lerp_mult = 1.0 / (R_cutoff - S_cutoff);
    //Loop to calculate PERIOD_SAMPLES number of samples
    for (int i = 0; i < PERIOD_SAMPLES; i++)
    {
        int currentSample = 0;
        //Loop through each note separately

        for (int current_note = 0; current_note < TOTAL_NUMBER_NOTES; current_note++)
        {
            int tempSample = 0;
            int note_done = 0;
            float x = 0;
            //get this notes attenuation vector
            //               usb_data[current_note].attenuation_vector
            float attenuation_multiple = 1.0;
            switch (usb_data[current_note].state)
            {
            case A:
                if (++usb_data[current_note].attenuation_vector >= A_cutoff)
                {
                    usb_data[current_note].state = D;
                }
                attenuation_multiple = (float)((usb_data[current_note].attenuation_vector) * A_lerp_mult);
                break;

            case D:
                if (++usb_data[current_note].attenuation_vector >= D_cutoff)
                {
                    usb_data[current_note].state = S;
                }
                x = (usb_data[current_note].attenuation_vector - A_cutoff) * D_lerp_mult;
                attenuation_multiple = 0.9 * (1.0 - x) + 1.0 * (x);
                break;

            case S:
                attenuation_multiple = 0.9;
                break;
            case R:
                if (++usb_data[current_note].attenuation_vector >= R_cutoff)
                {
                    usb_data[current_note].state = off;
                    note_done = 1;
                }
                //attenuation_multiple = 0.9 - (float)((usb_data[current_note].attenuation_vector - D_cutoff) * 0.00002267);
                x = (usb_data[current_note].attenuation_vector - S_cutoff) * R_lerp_mult;
                attenuation_multiple = 0.9 * (0.9 - x);
                break;
            }
            /*
            //Determine the attenuation factor based on the attenuation vector
            if (current_attenuation_vector < 44100)
            {
                attenuation_multiple = 1.0;
            }
            else if (current_attenuation_vector < 88200)
            {
                attenuation_multiple = 1 - (float)((current_attenuation_vector - 44100) * 0.00002267);
            }
            else
            {
                attenuation_multiple = 0.0;
            }
            */

            //Loop through the accumulators for those notes
            if (note_done)
            {
                for (int current_accumulator = (current_note * numHarmonics); current_accumulator < ((current_note + 1) * numHarmonics); current_accumulator++)
                {
                    dds_data[current_accumulator].enable = 0;
                }
            }
            else
            {
                for (int current_accumulator = (current_note * numHarmonics); current_accumulator < ((current_note + 1) * numHarmonics); current_accumulator++)
                {
                    if (dds_data[current_accumulator].enable)
                    {
                        accumulators[current_accumulator] += dds_data[current_accumulator].tuning_word;
                        accumulators[current_accumulator] = (accumulators[current_accumulator] > WAVETABLE_LENGTH) ? accumulators[current_accumulator] -= WAVETABLE_LENGTH : accumulators[current_accumulator];
                        //if note is enabled, currentSample += wavetable lookup
                        //bitwise anded with attenuation factor
                        tempSample += wavetable[accumulators[current_accumulator]] * dds_data[current_accumulator].attenuate;
                    }
                }
                tempSample = (int)((float)tempSample * attenuation_multiple) >> 1;
                currentSample = currentSample + tempSample;
            }
        }
        samples[i] = currentSample;
    }
}
