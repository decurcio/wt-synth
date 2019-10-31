#include <stdio.h>
#include <string.h>
#include "wavesynth.h"

#include "USB.h"

int tuning_lookup[] = {
	3110, 3295, 3491, 3698, 3918, 4151, 4398, 4661, 4937, 5230, 5542, 5871, 6220, 6590, 6982, 7397, 7837, 8303, 8797, 9320, 9874,
	10461, 11084, 11743, 12441, 13181, 13965, 14795, 15975, 16607, 17594, 18641, 19749, 20923, 22168, 23486, 24882, 26362, 27930,
	29590, 31350, 33214, 35189, 37282, 39499, 41847, 44336, 46972, 49765, 52725, 55860, 59181, 62701, 66429, 70379, 74564, 78998, 83695,
	88672, 93945, 99531, 105450, 111720, 118363, 125402, 132858, 140759, 149129, 157996, 167391, 177345, 187890, 199063, 210900,
	223441, 236727, 250804, 265717, 281518, 298258, 315993, 334783, 354690, 375781, 398126, 421800, 446882, 473455, 501608, 531435, 563036,
	596516, 631986, 669566, 709381, 751563, 796253, 843601, 893764, 946910, 1003216, 1062871, 112072, 1193032, 1263973, 1339133,
	1418762, 1503126, 1592507, 1687202, 4787529, 1893821, 2006433, 2125742, 2252145, 23860651, 2527947, 2678267, 2837525, 3006253, 3185014,
	3374405, 3575058, 3787642, 4012867, 4251484, 4504291, 4772130};

int note_buffer[TOTAL_NUMBER_FREQUENCIES];
int note_buffer_ptr = 0;
int config_buffer[4];
int config_buffer_ptr = 0;

void *usb(void *args)
{
	//Get the data from the void*
	USB_thread_args *USB_args = (USB_thread_args *)args;		 //Get the arguments that are passed in
	DDS_data *data = USB_args->data;							 //The dds data struct. Is an array of frequency information
	USB_data *USB_data = USB_args->usb_data_array;				 //The usb data struct. stores information on midi notes
	instrument *instrumentArray = *(USB_args->instrumentArray);  //Instrument array. Stores all the instruments read in
	instrument *currentInstrument = USB_args->currentInstrument; //The currently selected instrument
	int numberInstruments = USB_args->numberInstruments;		 //The total number of instruments from the config file

	//The default instrument is 0
	*currentInstrument = instrumentArray[0];
	printf("Current instrument: %s\n", currentInstrument->name);
	//The current number of notes being played
	int numNotesBeingPlayed = 0;
	char *buffer;
	size_t bufsize = 100;
	size_t characters;
	int i = 0;

	int command_int = 0;
	int key_int = 0;
	int velocity_int = 0;

	buffer = (char *)malloc(bufsize * sizeof(char));
	if (buffer == NULL)
	{
		perror("Unable to allocate buffer");
		exit(1);
	}
	int recent_note;
	int note_count;
	int config_mode = 0;
	while (1)
	{
		characters = getline(&buffer, &bufsize, stdin);
		//printf("You typed: %s\n", buffer);
		const char s[5] = "[], ";
		char *token;
		i = 0;
		token = strtok(buffer, s);
		while ((token != NULL) && (i < 3))
		{
			switch (i)
			{
			case 0:
				command_int = atoi(token);
				break;
			case 1:
				key_int = atoi(token);
				break;
			case 2:
				velocity_int = atoi(token);
				break;
			default:
				break;
			}
			printf("%d\n", atoi(token));
			token = strtok(NULL, s);
			i++;
		}
		printf("Command is %d, Key is %d, and Velocity is %d.\n", command_int, key_int, velocity_int);
		//printf(" The values for command is %d", command_int);
		//Note OFF event
		if ((command_int == 128) || (velocity_int == 0))
		{
			printf("NOTE OFF COMMAND\n");
			//loop through all of the notes in the note queue
			for (int i = 0; i < TOTAL_NUMBER_NOTES; i++)
			{
				//the current note in the queue is the same as the one to be stopped
				if (USB_data[i].midi_note == key_int)
				{
					//printf("REMOVING FROM SLOT %i\n", i);
					//disable that note
					USB_data[i].midi_note = -1;
					//loop through dds_data disabling those harmonics
					/*
					for (int k = (i * currentInstrument->numHarmonics); k < ((i + 1) * currentInstrument->numHarmonics); k++)
					{
						data[k].enable = 0;
					}
					*/
					int currentNoteAge = USB_data[i].age;
					//loop through all the notes and decrement any with an age newer than the current one
					for (int j = 0; j < TOTAL_NUMBER_NOTES; j++)
					{
						if (USB_data[j].age > currentNoteAge)
						{
							USB_data[j].age--;
						}
					}
					USB_data[i].age = 0;
					--numNotesBeingPlayed;
					USB_data[i].state = R;
					//printf("NUM CURRENT NOTES: %i\n", numNotesBeingPlayed);
					//printf("MIDI VALUE: %i\n", USB_data[i].midi_note);
				}
			}
		}
		//Note ON event
		else if (command_int == 144 && config_mode != 1)
		{
			printf("NOTE ON COMMAND\n");
			//fprintf(stderr, "NUM HARMONICS: %i\n", currentInstrument.numHarmonics);
			
			//Calculate the velocity multiple 
			float velocity_multiple = (float)velocity_int / 127.0;
			//Find either an open slot or the oldest (lowest age)
			int openNoteSlot = 0;
			int oldestNote = 0;
			while (openNoteSlot < TOTAL_NUMBER_NOTES)
			{
				if (USB_data[openNoteSlot].state == off)
				{
					//Found an open slot
					//printf("OPEN SLOT: %i\n", openNoteSlot);
					break;
				}
				else if (USB_data[openNoteSlot].age < USB_data[oldestNote].age)
				{
					//There is a new oldest note
					//printf("NEW OLDEST: %i\n", openNoteSlot);
					oldestNote = openNoteSlot;
				}
				openNoteSlot++;
			}
			int slot;
			//If openNoteSlot == 10, no open slot was found
			//replace the oldest
			if (openNoteSlot == 10)
			{
				//printf("REPLACE THE OLDEST AT: %i\n", oldestNote);
				slot = oldestNote;
				USB_data[oldestNote].midi_note = key_int;
				USB_data[oldestNote].attenuation_vector = 0;
				USB_data[oldestNote].age = numNotesBeingPlayed;
				USB_data[oldestNote].state = A;
				for (int k = 0; k < TOTAL_NUMBER_NOTES; k++)
				{
					if (k != slot)
					{
						USB_data[k].age--;
					}
				}
			}
			//Otherwise there is an open slot, replace that slot
			else
			{
				//printf("REPLACE OPEN SLOT AT: %i\n", openNoteSlot);
				slot = openNoteSlot;
				USB_data[openNoteSlot].midi_note = key_int;
				USB_data[openNoteSlot].attenuation_vector = 0;
				USB_data[openNoteSlot].age = ++numNotesBeingPlayed;
				USB_data[openNoteSlot].state = A;
			}
			//printf("SLOT NUMBER %i\n", slot);
			//printf("NUM CURRENT NOTES: %i\n", numNotesBeingPlayed);
			//loop through the harmonics of the instrument at the location
			//of the corresponding note's bucket in the DDS data array
			int DDS_data_bucket_location = (currentInstrument->numHarmonics * slot);
			for (int i = DDS_data_bucket_location; i < DDS_data_bucket_location + currentInstrument->numHarmonics; i++)
			{
				int currentHarmonic = i - DDS_data_bucket_location;
				data[i].tuning_word = (int)((float)tuning_lookup[key_int] * currentInstrument->harmonicMultiples[currentHarmonic]);
				//fprintf(stderr, "%i\n", data[i].tuning_word);
				data[i].attenuate = currentInstrument->attenuationMultiples[currentHarmonic] * velocity_multiple;
				data[i].enable = 1;
				//printf("BUCKET LOCATION: %i, TUNING WORD: %i\n", i, data[i].tuning_word);
			}

			if (key_int == 53 || key_int == 55 || key_int == 83 || key_int == 84)
			{
				if (config_buffer_ptr < 4)
				{
					config_buffer[config_buffer_ptr] = key_int;
					config_buffer_ptr++;
				}
			}
			else
			{
				config_buffer_ptr = 0;
				config_buffer[0] = 0;
				config_buffer[1] = 0;
				config_buffer[2] = 0;
				config_buffer[3] = 0;
			}
			if (config_buffer_ptr >= 4)
			{
				if (config_buffer[0] == 53 || config_buffer[0] == 55 || config_buffer[0] == 83 || config_buffer[0] == 84)
				{
					fprintf(stderr, "NOTE 0: %i\n", config_buffer[0]);
					if (config_buffer[1] == 53 || config_buffer[1] == 55 || config_buffer[1] == 83 || config_buffer[1] == 84)
					{
						fprintf(stderr, "NOTE 1: %i\n", config_buffer[1]);
						if (config_buffer[2] == 53 || config_buffer[2] == 55 || config_buffer[2] == 83 || config_buffer[2] == 84)
						{
							fprintf(stderr, "NOTE 2: %i\n", config_buffer[2]);
							if (config_buffer[3] == 53 || config_buffer[3] == 55 || config_buffer[3] == 83 || config_buffer[3] == 84)
							{
								fprintf(stderr, "NOTE 3: %i\n", config_buffer[3]);
								config_mode = 1;
								printf("CONFIG MODE ENTERED\n");
								for (int i = 0; i < TOTAL_NUMBER_FREQUENCIES; i++)
								{
									data[i].enable = 0;
									//fprintf(stderr, "%i disabled\n", i);
								}
							}
						}
					}
				}
				config_buffer_ptr = 0;
				config_buffer[0] = 0;
				config_buffer[1] = 0;
				config_buffer[2] = 0;
				config_buffer[3] = 0;
			}
		}
		else if (command_int == 144 && config_mode)
		{

			//Loop through every instrument
			int currentInstrumentIndex = 0;
			int instrumentFound = 0;
			while ((currentInstrumentIndex != numberInstruments) && !instrumentFound)
			{
				if (instrumentArray[currentInstrumentIndex].midiValue == key_int)
				{
					//The current instrument matches the selected note
					instrumentFound = 1;
					*currentInstrument = instrumentArray[currentInstrumentIndex];
					printf("SELECTING INSTRUMENT %i\n", currentInstrumentIndex);
					printf("INSTRUMENT: %s\n", currentInstrument->name);
					break;
				}
				currentInstrumentIndex++;
			}
			if (!instrumentFound)
			{
				//The instrument has not been found, default to a sine wave
				printf("INSTRUMENT NOT FOUND, DEFAULT TO SINE WAVE");
				currentInstrument->numHarmonics = 1;
				currentInstrument->harmonicMultiples[0] = 1;
				currentInstrument->attenuationMultiples[0] = 1;
			}
			config_mode = 0;
			numNotesBeingPlayed = 0;
		}
	}
}
