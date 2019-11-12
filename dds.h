#ifndef DDS_FILE
#define DDS_FILE
void init_DDS();
void DDS(void *, void *, int, short *, short *, Envelope *);

int dds_flag = 0;
short samples[TOTAL_NUMBER_FREQUENCIES];
#endif
