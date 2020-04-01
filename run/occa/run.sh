#!/bin/bash

export SWE_OUTPUT_NAME="output"
export SWE_NUM_COMPUTE_UNITS=1
export SWE_NODES=2562
export SWE_NSTEPS=100
export SWE_HIST_INT=1000
export SWE_NATTEMPTS=10
export SWE_USE_RCM=1
export SWE_USE_NETCDF=0
export SWE_USE_OCCA=1
export SWE_USE_OCL=1

cd `dirname "${BASH_SOURCE[0]}"`
cd ../..
export SWE_TOP_DIR=$PWD
export SWE_OUTPUT_NAME="output"
export SWE_INPUT_FILE=$SWE_TOP_DIR/input_files/icos"$SWE_NODES"_tc5_input.bin
export SWE_RUN_DIR=$SWE_TOP_DIR/run/occa
export SWE_OUTPUT_DIR=$SWE_TOP_DIR/run/occa/output

mkdir -p $SWE_OUTPUT_DIR

echo "SWE_TOP_DIR="$SWE_TOP_DIR
echo "SWE_INPUT_FILE="$SWE_INPUT_FILE
echo "SWE_OUTPUT_DIR="$SWE_OUTPUT_DIR
echo "SWE_RUN_DIR="$SWE_RUN_DIR
echo "Default output being written to:"$SWE_OUTPUT_DIR/"$SWE_OUTPUT_NAME"_default.txt
$SWE_RUN_DIR/swe_occa.exe 

