#ifndef RK_OCCA_H
#define RK_OCCA_H

#include <profiling.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <swe_config.h>
#include <device_setup.h>

// TW: hack to doubles atm
#define occaFtype(a) occaDouble(a)

// Use kernels to calculate the Runge-Kutta K value and add together to solve the differential equation
void RK_substep_occa(occaKernel eval_combined_kernel,
		     occaKernel eval_RHS_kernel,
		     occaKernel copy_arr_kernel,
		     occaKernel update_D_kernel,
		     occaKernel eval_K_kernel,
		     occaKernel update_H_kernel,		     
		     occaDevice device,
		     LPSMD_buffers *LPSMD_buffs,
		     occaMemory H_buff,
		     occaMemory F_buff,
		     occaMemory K_buff,
		     occaMemory K2_buff,
		     occaMemory D_buff,
		     int substep_id) ;


// Calculate righthand side of the differential equation
void eval_RHS_occa(occaKernel kernel, occaDevice device,
		   LPSMD_buffers *LPSMD_buffs, occaMemory K_buff, occaMemory F_buff);

void copy_arr_occa(occaKernel, occaDevice, occaMemory D_buff, occaMemory F_buff);

// Adds each RK K to D- D stores weighted sum of K values for each timestep
void update_D_occa(occaKernel, occaDevice, occaMemory, occaMemory, const fType);

// Calculate the RK K value
void eval_K_occa(occaKernel kernel, occaDevice device,
		 occaMemory H_buff, occaMemory F_buff, occaMemory K_buff,
		 const fType dt);

// Add weighted sum of K values to H
void update_H_occa(occaKernel kernel, occaDevice device,
		   occaMemory H_buff,occaMemory D_buff,occaMemory K_buff, const fType dt);


void eval_combined_occa(occaKernel kernel, occaDevice device,
			LPSMD_buffers *LPSMD_buffs,
			occaMemory K_buff, occaMemory F_buff, occaMemory D_buff, occaMemory H_buff, occaMemory K2_buff,
			double sc1, double sc2);

#endif
