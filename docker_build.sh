#!/bin/sh

#
# Script to build the daemon on a Docker container with a GCC compiler for ARMv7.
#

set -e

readonly CONTAINER_NAME="built_caller_daemon"
readonly IMAGE_NAME="gcc_arm"
readonly OUTPUT_DIR='out'

# Clean
rm -fr ${OUTPUT_DIR}
mkdir -p ${OUTPUT_DIR}

# Build using an image of GCC for ARMv7
docker build -t ${IMAGE_NAME} .

# Run it to access the file built
docker run --rm --detach --name ${CONTAINER_NAME} ${IMAGE_NAME}

# Copy the file from the running container
docker cp ${CONTAINER_NAME}:/usr/src/out/callblockerd ./${OUTPUT_DIR}/callblockerd_armv7

# Stop the container which is sleeping
echo "Stopping the container..."
docker stop -t 0 ${CONTAINER_NAME}

echo "Build done!"
