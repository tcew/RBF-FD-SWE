#ifndef RK_OCCA_H
#define RK_OCCA_H

#include <profiling.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <swe_config.h>
#include <device_setup.h>

#if sizeof(fType)==4
#define occaFtype(a) occaFloat(a)
#else
#define occaFtype(a) occaDouble(a)
#endif


// Use kernels to calculate the Runge-Kutta K value and add together to solve the differential equation
void RK_substep_occa(occaKernel, occaKernel, occaKernel, occaKernel, occaKernel, occaDevice,
		     occaMemory H_buff,occaMemory F_buff,occaMemory K_buff,occaMemory D_buff,
		     int);

// Calculate righthand side of the differential equation
void eval_RHS_ocl(occaKernel, occaDevice);

void copy_arr_ocl(occaKernel, occaDevice);

// Adds each RK K to D- D stores weighted sum of K values for each timestep
void update_D_ocl(occaKernel, occaDevice, const fType);

// Calculate the RK K value
void eval_K_ocl(occaKernel, occaDevice, const fType);

// Add weighted sum of K values to H
void update_H_ocl(occaKernel, occaDevice);

#endif
