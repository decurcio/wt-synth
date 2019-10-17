#include "wavesynth.h"

int accumulators[TOTAL_NUMBER_FREQUENCIES];
void init_DDS(short* samples) {
    //Initialization function for DDS
    for (int i = 0; i < TOTAL_NUMBER_FREQUENCIES; i++) {
        accumulators[i] = 0;
        samples[i] = 0;
    }
}

void DDS(void *DDS_data_array, short *wavetable, short* samples) {
    DDS_data *dds_data = DDS_data_array;
    //Loop to calculate PERIOD_SAMPLES number of samples
    for(int i = 0; i < PERIOD_SAMPLES; i++) {
        int currentSample = 0;
        //loop through the accumulator register, calculating new lookup values
        //Calculate the current sample from looked up samples and attenuations
        for (int j = 0; j < TOTAL_NUMBER_FREQUENCIES; j++) {
            accumulators[j]+=dds_data[j].tuning_word;
            accumulators[j] = (accumulators[j] > WAVETABLE_LENGTH) ?
                              accumulators[j]-=WAVETABLE_LENGTH : accumulators[j];
            if(dds_data[j].enable) {
                //if note is enabled, currentSample += wavetable lookup
                //bitwise anded with attenuation factor
                currentSample += wavetable[accumulators[j]]  >> (((dds_data[j].attenuate) + 2) );
            }
        }
        samples[i] = (short)currentSample;
    }

} /* DDS */
