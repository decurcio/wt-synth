#ifndef SYNTHESIZER
#define SYNTHESIZER
#include <string.h>
//Definitions
#define TOTAL_NUMBER_FREQUENCIES 120 //Total number of frequencies possible
#define TOTAL_NUMBER_NOTES 10		 //Total number of notes that can be played at once
#define SAMPLE_WIDTH (2)			 // number of bytes
#define WAVETABLE_LENGTH (16777216)  // Length
#define PERIOD_SAMPLES (64)			 //number of samples per period, mono

enum note_state
{
	off,
	A,
	D,
	S,
	R
};
//Data structures
typedef struct
{
	//Cutoff values (number of samples)
    int A_cutoff;
    int D_cutoff;
    int S_cutoff;
    int R_cutoff;

	float sustain_level;

    //attenuation calculation values
    float A_lerp_mult;
    float D_lerp_mult;
    float R_lerp_mult;
} Envelope;
typedef struct
{
	//synthesizer data
	unsigned tuning_word;
	float attenuate;
	char enable;
} DDS_data;
typedef struct
{
	//USB data struct; holds information for 1 midi note
	int midi_note;
	int attenuation_vector;
	int age;
	enum note_state state;
	float current_attenuation;
} USB_data;

typedef struct
{
	//Instrument struct; holds the data for one instrument
	char *name;
	int attenuationVector;
	int numHarmonics;
	int midiValue;
	float *harmonicMultiples;
	float *attenuationMultiples;
} instrument;

typedef struct
{
	//struct to pass data to the usb thread
	DDS_data *data;
	USB_data *usb_data_array;
	instrument **instrumentArray;
	instrument *currentInstrument;
	Envelope *envelope;
	int numberInstruments;

} USB_thread_args;


//Function declarations
void *usb(void *args);

#endif
