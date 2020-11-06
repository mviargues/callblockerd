#!/bin/sh

# Compile the callblocker deamon locally.
# Use docker_build.sh to build for ARM if you are not on an ARM machine.

set -e

echo "Compiling..."

OUTPUT_DIR='out'
if [[ ! -d ${OUTPUT_DIR} ]]; then
    mkdir ${OUTPUT_DIR}
fi

pushd src
gcc *.cpp -lstdc++ -o ../${OUTPUT_DIR}/callblockerd
popd
