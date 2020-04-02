// Written by Richelle Streater, Summer 2018.
// Modified by Tim Warburton, Spring 2020.

#ifdef USE_OCCA

#include <buffers.h>

extern PSMD_struct* LPSMD;

// Create device copy of host array
void load_buffer_fType(occaDevice device, occaMemory* buff, fType* buff_source, int buff_size) {
  *buff = occaDeviceMalloc(device, sizeof(fType)*buff_size, buff_source, occaDefault);
}

// Create device copy of 2D host array of fTypes transposed
void load_buffer_fType_transposed(occaDevice device, occaMemory *buff, fType* buff_source, int buff_nrows, int buff_ncols) {

  fType *tmp = (fType*) calloc(buff_nrows*buff_ncols, sizeof(fType));
  for(int i=0;i<buff_nrows;++i){
    for(int j=0;j<buff_ncols;++j){
      tmp[i+j*buff_nrows] = buff_source[i*buff_ncols+j];
    }
  }
  *buff = occaDeviceMalloc(device, sizeof(fType)*buff_nrows*buff_ncols, tmp, occaDefault);

  free(tmp);
}

// Create device copy of host array of ints
void load_buffer_int(occaDevice device, occaMemory *buff, int* buff_source, int buff_size) {
  *buff = occaDeviceMalloc(device, sizeof(int)*buff_size, buff_source, occaDefault);
}

// Create device copy of 2D host array of ints transposed
void load_buffer_int_transposed(occaDevice device, occaMemory *buff, int* buff_source, int buff_nrows, int buff_ncols) {

  int *tmp = (int*) calloc(buff_nrows*buff_ncols, sizeof(int));
  for(int i=0;i<buff_nrows;++i){
    for(int j=0;j<buff_ncols;++j){
      tmp[i+j*buff_nrows] = buff_source[i*buff_ncols+j];
    }
  }
  *buff = occaDeviceMalloc(device, sizeof(int)*buff_nrows*buff_ncols, tmp, occaDefault);

  free(tmp);
}

// Copy input arrays to buffers and initialize the RK kernel inputs
void load_all_buffers(occaDevice device, LPSMD_buffers* LPSMD_buffs, occaMemory* F_buff, occaMemory* D_buff) {
    int padded_Nnodes = LPSMD->compute_size;
    int padded_Nnbr = LPSMD->padded_Nnbr;
    int Nnbr = LPSMD->Nnbr;
    
    load_buffer_int  (device, &LPSMD_buffs->idx,LPSMD->idx,padded_Nnodes * padded_Nnbr);
    load_buffer_fType(device, &LPSMD_buffs->Dx, LPSMD->Dx, padded_Nnodes * padded_Nnbr);
    load_buffer_fType(device, &LPSMD_buffs->Dy, LPSMD->Dy, padded_Nnodes * padded_Nnbr);
    load_buffer_fType(device, &LPSMD_buffs->Dz, LPSMD->Dz, padded_Nnodes * padded_Nnbr);
    load_buffer_fType(device, &LPSMD_buffs->L,  LPSMD->L,  padded_Nnodes * padded_Nnbr);

    load_buffer_int_transposed  (device, &LPSMD_buffs->idxT,LPSMD->idx,padded_Nnodes, padded_Nnbr);
    load_buffer_fType_transposed(device, &LPSMD_buffs->DxT, LPSMD->Dx, padded_Nnodes, padded_Nnbr);
    load_buffer_fType_transposed(device, &LPSMD_buffs->DyT, LPSMD->Dy, padded_Nnodes, padded_Nnbr);
    load_buffer_fType_transposed(device, &LPSMD_buffs->DzT, LPSMD->Dz, padded_Nnodes, padded_Nnbr);
    load_buffer_fType_transposed(device, &LPSMD_buffs->LT,  LPSMD->L,  padded_Nnodes, padded_Nnbr);

    load_buffer_fType(device, &LPSMD_buffs->x, LPSMD->x, padded_Nnodes);
    load_buffer_fType(device, &LPSMD_buffs->y, LPSMD->y, padded_Nnodes);
    load_buffer_fType(device, &LPSMD_buffs->z, LPSMD->z, padded_Nnodes);
    load_buffer_fType(device, &LPSMD_buffs->f, LPSMD->f, padded_Nnodes);
    load_buffer_fType(device, &LPSMD_buffs->ghm, LPSMD->ghm, padded_Nnodes);
    load_buffer_fType(device, &LPSMD_buffs->p_u, LPSMD->p_u, padded_Nnodes * 3);
    load_buffer_fType(device, &LPSMD_buffs->p_v, LPSMD->p_v, padded_Nnodes * 3);
    load_buffer_fType(device, &LPSMD_buffs->p_w, LPSMD->p_w, padded_Nnodes * 3);
    load_buffer_fType(device, &LPSMD_buffs->gradghm, LPSMD->gradghm, padded_Nnodes * 3);
    
    // F and D start out as NULL
    *F_buff = occaDeviceMalloc(device, sizeof(fType)*padded_Nnodes*4, NULL, occaDefault);
    *D_buff = occaDeviceMalloc(device, sizeof(fType)*padded_Nnodes*4, NULL, occaDefault);
}

// Free memory for all buffers
void release_all_buffers(LPSMD_buffers* LPSMD_buffs, occaMemory* H_buff, occaMemory* F_buff, occaMemory* D_buff, occaMemory* K_buff) {

  #if 0
    // For each buffer, check if it exists, and if not, release
    if(LPSMD_buffs->x) occaFree(LPSMD_buffs->x);
    if(LPSMD_buffs->y) occaFree(LPSMD_buffs->y);
    if(LPSMD_buffs->z) occaFree(LPSMD_buffs->z);
    if(LPSMD_buffs->f) occaFree(LPSMD_buffs->f);
    if(LPSMD_buffs->ghm) occaFree(LPSMD_buffs->ghm);
    if(LPSMD_buffs->p_u) occaFree(LPSMD_buffs->p_u);
    if(LPSMD_buffs->p_v) occaFree(LPSMD_buffs->p_v);
    if(LPSMD_buffs->p_w) occaFree(LPSMD_buffs->p_w);
    if(LPSMD_buffs->gradghm) occaFree(LPSMD_buffs->gradghm);
    if(LPSMD_buffs->idx) occaFree(LPSMD_buffs->idx);
    if(LPSMD_buffs->Dx) occaFree(LPSMD_buffs->Dx);
    if(LPSMD_buffs->Dy) occaFree(LPSMD_buffs->Dy);
    if(LPSMD_buffs->Dz) occaFree(LPSMD_buffs->Dz);
    if(LPSMD_buffs->L) occaFree(LPSMD_buffs->L);
    
    if(*F_buff) occaFree(*F_buff);
    if(*K_buff) occaFree(*K_buff);
    if(*D_buff) occaFree(*D_buff);
    if(*H_buff) occaFree(*H_buff);
    #endif
}

#endif
