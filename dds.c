#include "wavesynth.h"

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
void DDS(void *DDS_data_array, void *USB_data_array, instrument *currentInstrument, short *wavetable, short *samples)
{
    DDS_data *dds_data = DDS_data_array;
    USB_data *usb_data = USB_data_array;
    
    //Loop to calculate PERIOD_SAMPLES number of samples
    for (int i = 0; i < PERIOD_SAMPLES; i++)
    {
        int currentSample = 0;
        //Loop through each note separately
        for(int current_note = 0; current_note < TOTAL_NUMBER_NOTES; current_note++) {
            //get this notes attenuation vector
            int current_attenuation_vector = usb_data[current_note].attenuation_vector += 1;
            float attenuation_multiple = 1.0;
            //Determine the attenuation factor based on the attenuation vector
            if(current_attenuation_vector < 44100) {
                attenuation_multiple = 1.0;
            } else if(current_attenuation_vector < 88200) {
                attenuation_multiple = 1 - (float)((current_attenuation_vector - 44100) * 0.00002267); 
            } else {
                attenuation_multiple = 0.0;
            }

            //Loop through the accumulators for those notes
            for(int current_accumulator = (current_note * currentInstrument->numHarmonics); current_accumulator < ((current_note + 1) * currentInstrument->numHarmonics);current_accumulator++) {
                accumulators[current_accumulator] += dds_data[current_accumulator].tuning_word;
                accumulators[current_accumulator] = (accumulators[current_accumulator] > WAVETABLE_LENGTH) ? accumulators[current_accumulator] -= WAVETABLE_LENGTH : accumulators[current_accumulator];
                if (dds_data[current_accumulator].enable)
                {
                    //if note is enabled, currentSample += wavetable lookup
                    //bitwise anded with attenuation factor
                    currentSample += wavetable[accumulators[current_accumulator]] * dds_data[current_accumulator].attenuate;
                }
            }
        samples[i] += (short)((float)currentSample * attenuation_multiple);
        }



        /*
        //increment the note's attenuation vector
        int current_attenuation_vector = usb_data->attenuation_vector += 1;
        float attenuation_multiple = 1.0;
        //Determine the attenuation factor based on the attenuation vector
        if(current_attenuation_vector < 44100) {
            attenuation_multiple = 1.0;
        } else if(current_attenuation_vector < 88200) {
            attenuation_multiple = 1 - (float)((current_attenuation_vector - 44100) * 0.00002267); 
        } else {
            attenuation_multiple = 0.0;
        }
        //loop through the accumulator register, calculating new lookup values
        //Calculate the current sample from looked up samples and attenuations
        for (int j = 0; j < TOTAL_NUMBER_FREQUENCIES; j++)
        {
            accumulators[j] += dds_data[j].tuning_word;
            accumulators[j] = (accumulators[j] > WAVETABLE_LENGTH) ? accumulators[j] -= WAVETABLE_LENGTH : accumulators[j];
            if (dds_data[j].enable)
            {
                //if note is enabled, currentSample += wavetable lookup
                //bitwise anded with attenuation factor
                currentSample += wavetable[accumulators[j]] * dds_data[j].attenuate;
            }
        }
        samples[i] = (short)((float)currentSample * attenuation_multiple);
    }
    */

} /* DDS */
