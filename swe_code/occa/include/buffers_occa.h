#ifndef BUFFERS_H
#define BUFFERS_H

#include <device_setup_occa.h>
#include <stdlib.h>
#include <string.h>
#include <swe_config.h>

// Creates buffer buff with contents of buff_source
void load_buffer_fType(occaDevice, occaMemory* buff, fType* buff_source, int buff_size);

void load_buffer_int(occaDevice, occaMemory* buff, int* buff_source, int buff_size);

void load_all_buffers(occaDevice, LPSMD_buffers* LPSMD_buffs, occaMemory* F_buff, occaMemory* D_buff);

void release_all_buffers(LPSMD_buffers* LPSMD_buffs, occaMemory* H_buff, occaMemory* F_buff, occaMemory* D_buff, occaMemory* K_buff);
#endif
