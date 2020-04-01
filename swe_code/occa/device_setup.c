// Written by Tuan Ta, 2016- from Gen1 RBF_SWE code. Last updated- Richelle Streater, Summer 2018.

#ifdef USE_OCCA

extern sim_params_struct sim_params;

extern int mpi_rank;
extern int mpi_size;

// Set all inputs for the kernel that evaluates the SWE righthand side
void set_eval_RHS_args(cl_kernel kernel, LPSMD_buffers* LPSMD_buffs, cl_mem K_buff, cl_mem F_buff, fType gh0, int Nnodes){
    cl_int err = 0;
	
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&(K_buff));
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&(LPSMD_buffs->idx));
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&(LPSMD_buffs->Dx));
    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&(LPSMD_buffs->Dy));
    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&(LPSMD_buffs->Dz));
    err = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&(LPSMD_buffs->L));
    err = clSetKernelArg(kernel, 6, sizeof(cl_mem), (void*)&(LPSMD_buffs->x));
    err = clSetKernelArg(kernel, 7, sizeof(cl_mem), (void*)&(LPSMD_buffs->y));
    err = clSetKernelArg(kernel, 8, sizeof(cl_mem), (void*)&(LPSMD_buffs->z));
    err = clSetKernelArg(kernel, 9, sizeof(cl_mem), (void*)&(LPSMD_buffs->f));
    err = clSetKernelArg(kernel, 10, sizeof(cl_mem), (void*)&(LPSMD_buffs->ghm));
    err = clSetKernelArg(kernel, 11, sizeof(cl_mem), (void*)&(LPSMD_buffs->p_u));
    err = clSetKernelArg(kernel, 12, sizeof(cl_mem), (void*)&(LPSMD_buffs->p_v));
    err = clSetKernelArg(kernel, 13, sizeof(cl_mem), (void*)&(LPSMD_buffs->p_w));
    err = clSetKernelArg(kernel, 14, sizeof(cl_mem), (void*)&(LPSMD_buffs->gradghm));
    err = clSetKernelArg(kernel, 15, sizeof(cl_mem), (void*)&(F_buff));
    err = clSetKernelArg(kernel, 16, sizeof(fType), (void*)&gh0);
    err = clSetKernelArg(kernel, 17, sizeof(int), (void*)&Nnodes);
    err = clSetKernelArg(kernel, 18, sizeof(int), (void*)&padded_Nnbr);
    err = clSetKernelArg(kernel, 19, sizeof(int), (void*)&Nnbr);
    err = clSetKernelArg(kernel, 20, sizeof(int), (void*)&compute_pid_s);
}
    
// Set all inputs for the kernel that copies F's contents to D's
void set_copy_arr_args(cl_kernel kernel, cl_mem D_buff, cl_mem F_buff, const int arr_size){
    cl_int err = 0;
                             
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&D_buff);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&F_buff);
    err = clSetKernelArg(kernel, 2, sizeof(int), (void*)&arr_size);
}
                         
// Set all inputs for the kernel that updates D (represents differentiation matrix x velocity)
void set_update_D_args(cl_kernel kernel, cl_mem F_buff, cl_mem D_buff, const int Nnodes){
    cl_int err = 0;
                             
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&F_buff);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&D_buff);
    err = clSetKernelArg(kernel, 3, sizeof(int), (void*)&Nnodes);
}
                         
                         
// Set all inputs for the kernel that calculates K in the SWE's
void set_eval_K_args(cl_kernel kernel, cl_mem H_buff, cl_mem F_buff, const int Nnodes, cl_mem K_buff, int compute_pid_s){
    cl_int err = 0;

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&H_buff);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&F_buff);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&K_buff);
    err = clSetKernelArg(kernel, 4, sizeof(int), (void*)&Nnodes);
    err = clSetKernelArg(kernel, 5, sizeof(int), (void*)&compute_pid_s);
}

// Set arguments for the kernel that updates H in the SWE's
void set_update_H_args (cl_kernel kernel, cl_mem H_buff, cl_mem D_buff, cl_mem K_buff, const int Nnodes, const fType dt, int compute_pid_s){
    cl_int err = 0;

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&H_buff);
    err = clSetKernelArg(kernel, 1, sizeof(fType), (void*)&dt);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&D_buff);
    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&K_buff);
    err = clSetKernelArg(kernel, 4, sizeof(cl_int), (void*)&Nnodes);
    err = clSetKernelArg(kernel, 5, sizeof(cl_int), (void*)&compute_pid_s);
}

#endif
