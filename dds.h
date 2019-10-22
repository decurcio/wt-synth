#ifndef DDS_FILE
#define DDS_FILE
void init_DDS();
void DDS(void* , void*, short*, short*);

int dds_flag = 0;
short samples[TOTAL_NUMBER_FREQUENCIES];
#endif
