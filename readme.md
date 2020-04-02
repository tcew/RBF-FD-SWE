To install OCCA:

``` 
git clone https://github.com/libocca/occa
cd occa
export OCCA_DIR=`pwd`
make -j
````

To install this RBF-FD-SWE fork:

```
cd ~/
git clone https://github.com/tcew/RBF-FD-SWE
cd RBF-FD-SWE
# edit  swe_code/main/main.c to select the OCCA mode you want
CONFIG_DIR=occa make EXEC=swe_occa.exe
# to run 
cd run/occa
./run.sh
```

You should see output that ends with this type of output:

```
SWE:occaConfig=mode: 'HIP'  , device_id: 0
Attempt 1:	VERIFICATION SUCCEEDED
Attempt 2:	VERIFICATION SUCCEEDED
Attempt 3:	VERIFICATION SUCCEEDED
Attempt 4:	VERIFICATION SUCCEEDED
Attempt 5:	VERIFICATION SUCCEEDED
Attempt 6:	VERIFICATION SUCCEEDED
Attempt 7:	VERIFICATION SUCCEEDED
Attempt 8:	VERIFICATION SUCCEEDED
Attempt 9:	VERIFICATION SUCCEEDED
Attempt 10:	VERIFICATION SUCCEEDED

============================================================= Profiling Results =========================

Total Initialization Time (seconds): 	3.251600e-02
OCCA Setup Time (seconds): 	5.736687e+00
Main RK4 Loop (seconds/timestep) -> 	Average: 	5.528352e-04 	Min: 	5.358696e-04 	Max: 	6.432295e-04 	STDDEV: 	9.565072e-06
Eval_Rhs      (seconds/timestep) -> 	Average: 	3.272793e-04 	Min: 	3.240681e-04 	Max: 	3.290248e-04 	STDDEV: 	8.615679e-06
Eval_K        (seconds/timestep) -> 	Average: 	8.198810e-05 	Min: 	7.992029e-05 	Max: 	8.793354e-05 	STDDEV: 	6.601578e-07
Update_D      (seconds/timestep) -> 	Average: 	8.031535e-05 	Min: 	7.857084e-05 	Max: 	8.540392e-05 	STDDEV: 	5.576649e-07
Update_H      (seconds/timestep) -> 	Average: 	2.749586e-05 	Min: 	2.686739e-05 	Max: 	2.851248e-05 	STDDEV: 	1.260737e-07
MPI Overhead  (seconds/timestep) -> 	Average: 	0.000000e+00 	Min: 	0.000000e+00 	Max: 	0.000000e+00 	STDDEV: 	0.000000e+00

==========================================================================================================
```
