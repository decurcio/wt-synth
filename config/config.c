#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "config.h"
#include "../libs/json.h"

void init_wavetable(short **wavetable)
{

        *wavetable = malloc(SAMPLE_WIDTH * WAVETABLE_LENGTH);
        FILE *LUT_File;
        LUT_File = fopen("./wavetable_sine.bin", "rb"); // open file stream
        fread(*wavetable, SAMPLE_WIDTH, WAVETABLE_LENGTH, LUT_File);
}

int createInstrumentArray(int *numberInstruments, instrument **instrumentArray)
{
        char *filename;
        FILE *fp;
        struct stat filestatus;
        int file_size;
        char *file_contents;
        json_char *json;
        json_value *value;

        filename = "config/config.json";

        if (stat(filename, &filestatus) != 0)
        {
                fprintf(stderr, "File %s not found\n", filename);
                return 1;
        }
        file_size = filestatus.st_size;
        file_contents = (char *)malloc(filestatus.st_size);
        if (file_contents == NULL)
        {
                fprintf(stderr, "Memory error: unable to allocate %d bytes\n", file_size);
                return 1;
        }

        fp = fopen(filename, "rt");
        if (fp == NULL)
        {
                fprintf(stderr, "Unable to open %s\n", filename);
                fclose(fp);
                free(file_contents);
                return 1;
        }
        if (fread(file_contents, file_size, 1, fp) != 1)
        {
                fprintf(stderr, "Unable t read content of %s\n", filename);
                fclose(fp);
                free(file_contents);
                return 1;
        }
        fclose(fp);

        printf("%s\n", file_contents);

        printf("--------------------------------\n\n");

        json = (json_char *)file_contents;

        value = json_parse(json, file_size);

        if (value == NULL)
        {
                fprintf(stderr, "Unable to parse data\n");
                free(file_contents);
                exit(1);
        }
        //WAVESYNTH CODE
        //Get the number of instruments, and instantiate that number of structs to write to
        int totalInstruments = value->u.object.length;
        instrument *tempInstrumentArray = malloc(sizeof(instrument) * totalInstruments);
        //Get the current instrument json object
        for (int i = 0; i < totalInstruments; i++)
        {
                //Create a temporary instrument to stroe the values in
                json_value *currentInstrument = value->u.object.values[i].value;
                //Temporary pointer
                //Get the attenuation vector of the current instrument
                int nameLen = value->u.object.values[i].name_length;
                tempInstrumentArray[i].name = malloc(sizeof(char) * (nameLen + 1));
                for(int j = 0; j < nameLen; j++) {
                        tempInstrumentArray[i].name[j] = value->u.object.values[i].name[j];
                }
                tempInstrumentArray[i].name[nameLen + 1] = "\0";
                tempInstrumentArray[i].attenuationVector = currentInstrument->u.object.values[0].value->u.integer;
                //Get the number of harmonics this instrument has
                tempInstrumentArray[i].numHarmonics = currentInstrument->u.object.values[1].value->u.integer;
                tempInstrumentArray[i].midiValue = currentInstrument->u.object.values[2].value->u.integer;

                //instantiate the memory for the harmonics and attenuations for the current instrument
                tempInstrumentArray[i].harmonicMultiples = malloc(tempInstrumentArray[i].numHarmonics * sizeof(float));
                tempInstrumentArray[i].attenuationMultiples = malloc(tempInstrumentArray[i].numHarmonics * sizeof(float));

                //Loop through the number of harmonics and get the values for harmonicMultiple and attenuationMultiple
                for (int j = 0; j < tempInstrumentArray[i].numHarmonics; j++)
                {
                        tempInstrumentArray[i].harmonicMultiples[j] = (float)currentInstrument->u.object.values[3].value->u.array.values[j]->u.dbl;
                        tempInstrumentArray[i].attenuationMultiples[j] = (float)currentInstrument->u.object.values[4].value->u.array.values[j]->u.dbl;
                }
        }

        //return the number of instruments and the created instruments array
        *numberInstruments = totalInstruments;
        *instrumentArray = tempInstrumentArray;

        json_value_free(value);
        free(file_contents);
        return 0;
}
