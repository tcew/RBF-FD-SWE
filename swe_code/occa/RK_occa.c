// Written by Richelle Streater, Summer 2018- openCL version of rk4_rbffd_swe.c in main folder.

#ifdef USE_OCCA
#include <RK_occa.h>

extern PSMD_struct* LPSMD;
extern timing_struct local_timer;

void RK_substep_occa(occaKernel eval_RHS_kernel, occaKernel copy_arr_kernel, occaKernel update_D_kernel, occaKernel eval_K_kernel, occaKernel update_H_kernel, occaDevice device, int substep_id) {
    
    // full RK4 timestep length
    const fType dt = LPSMD->dt;
    
    switch(substep_id) {
        case 0:
            // get F_0 = d/dt(K_0 = H)
            eval_RHS_occa(eval_RHS_kernel, device);
            // initialize D = F_0
            copy_arr_occa(copy_arr_kernel, device);
            // evaluate K_1 = H + (dt/2) * F_0
            eval_K_occa(eval_K_kernel, device, dt/2.0);
            break;
            
        case 1:
            // get F_1 = d/dt(K_1 = H + (dt/2) * F_0)
            eval_RHS_occa(eval_RHS_kernel, device);
            // update D += 2 * F_1
            update_D_occa(update_D_kernel, device, 2.0);
            // evaluate K_2 = H + (dt/2) * F_0
            eval_K_occa(eval_K_kernel, device, dt/2.0);
            break;
            
        case 2:
            // get F_2 = d/dt(K_2 = H + (dt/2) * F_1)
            eval_RHS_occa(eval_RHS_kernel, device);
            // update D += 2 * F_2
            update_D_occa(update_D_kernel, device, 2.0);
            // evaluate K_3 = H + dt * F_0
            eval_K_occa(eval_K_kernel, device, dt);
            break;
            
        case 3:
            // get F_3 = d/dt(K_3 = H + dt * F_2)
            eval_RHS_occa(eval_RHS_kernel, device);
            // update D += F_3
            update_D_occa(update_D_kernel, device, 1.0);
            // add everything together to get H_n+1
            update_H_occa(update_H_kernel, device);
            break;
    }
}

void eval_RHS_occa(occaKernel kernel, occaDevice device,
		   occaMemory K_buff, occaMemory F_buff){
  
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
		  LPSMD_buffs->idx,
		  LPSMD_buffs->Dx,
		  LPSMD_buffs->Dy,
		  LPSMD_buffs->Dz,
		  LPSMD_buffs->L,
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
		  occaInt(LPSMD->compute_size),
		  occaInt(LPSMD->padded_Nnbr),
		  occaInt(LPSMD->Nnbr),
		  occaInt(LPSMD->compute_pid_s));

    // TW: NASTY ! Every invocation is blocking
    device.finish(); 
    local_timer.t_eval_rhs[local_timer.attempt] +=  (getTime() - t_start);
}

void copy_arr_occa(occaKernel kernel, occaDevice device,
		   occaMemory D_buff, occaMemory F_buff){
    
    // global size and work-group size
    int wgsize = 64;
    int wgcount = ((LPSMD->compute_size)*4)/ wgsize + 1;

    occaKernelRun(kernel, occaInt(wgcount), occaInt(wgsize), D_buff, F_buff, occaInt(4*LPSMD->compute_size));

    // add error checking later
    occaDeviceFinish(device);
}

void update_D_occa(occaKernel kernel, occaDevice device,
		   occaBuffer F_buff, occaBuffer D_buff, const fType coefficient, int Nnodes){
    double t_start = getTime();
    
    // global size and work-group size
    int wgsize = 64;
    int wgcount = ((LPSMD->compute_size)*4)/ wgsize + 1;

    occaRunKernel(kernel, occaInt(wgcount), occaInt(wgsize),
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

    occaRunKernel(kernel, occaInt(wgcount), occaInt(wgsize),
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

    occaRunKernel(kernel, occaInt(wgcount), occaInt(wgsize),
		  H_buff, occaFtype(dt), D_buff, K_buff, occaInt(LPSMD->compute_size), occaInt(LPSMD->compute_pid_s));
    
    // error check later
    occaDeviceFinish(device);
    local_timer.t_update_H[local_timer.attempt] +=  (getTime() - t_start);
}
#endif
