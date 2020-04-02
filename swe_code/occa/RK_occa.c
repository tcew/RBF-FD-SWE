// Written by Richelle Streater, Summer 2018- openCL version of rk4_rbffd_swe.c in main folder.

#ifdef USE_OCCA
#include <RK_occa.h>

extern PSMD_struct* LPSMD;
extern timing_struct local_timer;

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
		     int substep_id) {
    
  // full RK4 timestep length
  const fType dt = LPSMD->dt;
  
  switch(substep_id) {
  case 0:
    // get F_0 = d/dt(K_0 = H)
    eval_RHS_occa(eval_RHS_kernel, device, LPSMD_buffs, K_buff, F_buff); // F = RHS(K)
    // initialize D = F_0
    copy_arr_occa(copy_arr_kernel, device, D_buff, F_buff);              // D = F
    // evaluate K_1 = H + (dt/2) * F_0
    eval_K_occa(eval_K_kernel, device,  H_buff, F_buff, K_buff, dt/2.0); // K = H + (dt/2)*F
    break;
            
  case 1:{
#if 0
    // get F_1 = d/dt(K_1 = H + (dt/2) * F_0)
    eval_RHS_occa(eval_RHS_kernel, device, LPSMD_buffs, K_buff, F_buff); // F = RHS(K)
    // update D += 2 * F_1
    update_D_occa(update_D_kernel, device, F_buff, D_buff, 2.0);         // D += 2*F
    // evaluate K_2 = H + (dt/2) * F_0
    eval_K_occa(eval_K_kernel, device,  H_buff, F_buff, K_buff, dt/2.0); // K = H + (dt/2)*F
#else
    double sc1 = 2.;
    double sc2 = 0.5*dt;
    eval_combined_occa(eval_combined_kernel, device, LPSMD_buffs,
		       K_buff, F_buff, D_buff, H_buff, K2_buff, sc1, sc2);
#endif
    break;
  }            
  case 2:{
#if 0
    // get F_2 = d/dt(K_2 = H + (dt/2) * F_1)
    eval_RHS_occa(eval_RHS_kernel, device, LPSMD_buffs, K_buff, F_buff); // F = RHS(K)
    // update D += 2 * F_2
    update_D_occa(update_D_kernel, device, F_buff, D_buff, 2.0);         // D += 2*F
    // evaluate K_3 = H + dt * F_0
    eval_K_occa(eval_K_kernel, device, H_buff, F_buff, K_buff, dt);      // K = H + dt*F
#else
    double sc1 = 2.;
    double sc2 = dt;
    eval_combined_occa(eval_combined_kernel, device, LPSMD_buffs,
		       K2_buff, F_buff, D_buff, H_buff, K_buff, sc1, sc2);
#endif
    break;
  }  
  case 3:
    // get F_3 = d/dt(K_3 = H + dt * F_2)
    eval_RHS_occa(eval_RHS_kernel, device, LPSMD_buffs, K_buff, F_buff);
    // update D += F_3
    update_D_occa(update_D_kernel, device, F_buff, D_buff, 1.0);
    // add everything together to get H_n+1
    update_H_occa(update_H_kernel, device, H_buff, D_buff, K_buff, dt);
    break;
  }
}

void eval_RHS_occa(occaKernel kernel, occaDevice device,
		   LPSMD_buffers *LPSMD_buffs, occaMemory K_buff, occaMemory F_buff){
  
  double t_start = getTime();
    
  // The CFDL=1, SFDL=0 kernel is tiled differently than the other kernels
#if ((SIMD_LENGTH != 4) && (SIMD_LENGTH != 8)) || (defined(USE_CFDL) && !defined(USE_SFDL))
  int tile_length = 1;
    
#else
  int tile_length = SIMD_LENGTH;
#endif

  // global size and work-group size- wgsize is flexible within device limits.
  int wgsize = 128;
  int wgcount = ((LPSMD->compute_size)/tile_length)/wgsize + 1;

  occaKernelRun(kernel,
		occaInt(wgcount),
		occaInt(wgsize),
		K_buff,
		LPSMD_buffs->idxT, // using transposed array for coalescing
		LPSMD_buffs->DxT,
		LPSMD_buffs->DyT,
		LPSMD_buffs->DzT,
		LPSMD_buffs->LT,
		LPSMD_buffs->x,
		LPSMD_buffs->y,
		LPSMD_buffs->z,
		LPSMD_buffs->f,
		LPSMD_buffs->ghm,
		LPSMD_buffs->p_u,
		LPSMD_buffs->p_v,
		LPSMD_buffs->p_w,
		LPSMD_buffs->gradghm,
		F_buff,
		occaDouble(LPSMD->gh0),
		occaInt(LPSMD->compute_size), // padded_Nnodes
		occaInt(LPSMD->padded_Nnbr),
		occaInt(LPSMD->Nnbr),
		occaInt(LPSMD->compute_pid_s));

  // TW: NASTY ! Every invocation is blocking
  occaDeviceFinish(device);
  local_timer.t_eval_rhs[local_timer.attempt] +=  (getTime() - t_start);
}

void eval_combined_occa(occaKernel kernel, occaDevice device,
			LPSMD_buffers *LPSMD_buffs,
			occaMemory K_buff, occaMemory F_buff, occaMemory D_buff, occaMemory H_buff, occaMemory K2_buff,
			double sc1, double sc2){

  double t_start = getTime();
    
  // The CFDL=1, SFDL=0 kernel is tiled differently than the other kernels
#if ((SIMD_LENGTH != 4) && (SIMD_LENGTH != 8)) || (defined(USE_CFDL) && !defined(USE_SFDL))
  int tile_length = 1;
    
#else
  int tile_length = SIMD_LENGTH;
#endif

  // global size and work-group size- wgsize is flexible within device limits.
  int wgsize = 256; // changed from 128
  int wgcount = ((LPSMD->compute_size)/tile_length)/wgsize + 1;

  occaKernelRun(kernel,
		occaInt(wgcount),
		occaInt(wgsize),
		LPSMD_buffs->idxT, // using transposed array for coalescing
		LPSMD_buffs->DxT,
		LPSMD_buffs->DyT,
		LPSMD_buffs->DzT,
		LPSMD_buffs->LT,
		LPSMD_buffs->x,
		LPSMD_buffs->y,
		LPSMD_buffs->z,
		LPSMD_buffs->f,
		LPSMD_buffs->ghm,
		LPSMD_buffs->p_u,
		LPSMD_buffs->p_v,
		LPSMD_buffs->p_w,
		LPSMD_buffs->gradghm,
		occaDouble(sc1),
		occaDouble(sc2), 
		K_buff,
		F_buff,
		D_buff,
		H_buff,
		K2_buff,
		occaDouble(LPSMD->gh0),
		occaInt(LPSMD->compute_size), // padded_Nnodes
		occaInt(LPSMD->padded_Nnbr),
		occaInt(LPSMD->Nnbr),
		occaInt(LPSMD->compute_pid_s));

  // TW: NASTY ! Every invocation is blocking
  occaDeviceFinish(device);
  local_timer.t_eval_combined[local_timer.attempt] +=  (getTime() - t_start);
}





void copy_arr_occa(occaKernel kernel, occaDevice device,
		   occaMemory D_buff, occaMemory F_buff){
    
  // global size and work-group size
  int wgsize = 64;
  int wgcount = ((LPSMD->compute_size)*4)/ wgsize + 1;
  int cnt =4*LPSMD->compute_size;
  occaKernelRun(kernel, occaInt(wgcount), occaInt(wgsize), D_buff, F_buff, occaInt(cnt));

  // add error checking later
  occaDeviceFinish(device);
}

void update_D_occa(occaKernel kernel, occaDevice device,
		   occaMemory F_buff, occaMemory D_buff, const fType coefficient){
  double t_start = getTime();
    
  // global size and work-group size
  int wgsize = 64;
  int wgcount = ((LPSMD->compute_size)*4)/ wgsize + 1;

  occaKernelRun(kernel, occaInt(wgcount), occaInt(wgsize),
		F_buff, D_buff, occaFtype(coefficient), occaInt(LPSMD->compute_size));
    
  // error checking later
  occaDeviceFinish(device);
  local_timer.t_update_D[local_timer.attempt] += (getTime() - t_start);
}

void eval_K_occa(occaKernel kernel, occaDevice device,
		 occaMemory H_buff, occaMemory F_buff, occaMemory K_buff,
		 const fType dt){

  double t_start = getTime();
    
  // global size and work-group size
  int wgsize = 64;
  int wgcount = ((LPSMD->compute_size)*4)/ wgsize + 1;

  occaKernelRun(kernel, occaInt(wgcount), occaInt(wgsize),
		H_buff, F_buff, K_buff, occaDouble(dt), occaInt(LPSMD->compute_size), occaInt(LPSMD->compute_pid_s));
		  
  // error check later
  occaDeviceFinish(device);
  local_timer.t_eval_K[local_timer.attempt] +=  (getTime() - t_start);
}

void update_H_occa(occaKernel kernel, occaDevice device,
		   occaMemory H_buff,occaMemory D_buff,occaMemory K_buff, const fType dt){

  double t_start = getTime();
    
  // global size and work-group size
  int wgsize = 64;
  int wgcount = ((LPSMD->compute_size)*4)/wgsize + 1;

  occaKernelRun(kernel, occaInt(wgcount), occaInt(wgsize),
		H_buff, occaFtype(dt), D_buff, K_buff, occaInt(LPSMD->compute_size), occaInt(LPSMD->compute_pid_s));
    
  // error check later
  occaDeviceFinish(device);
  local_timer.t_update_H[local_timer.attempt] +=  (getTime() - t_start);
}
#endif
