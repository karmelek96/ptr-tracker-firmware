#!/bin/bash

# Script to compile tracker_firmware on any platform using Docker
# This script is copied into the Docker image when it is built.
# If you plan to change this script, please make sure to rebuild the Docker image.

set -e # Exit if failed
SOURCE_PATH="/usr/local/src/tracker_firmware"

# Sanity check
if [ ! -d "${SOURCE_PATH}" ]
then
    echo "Source directory does not exist, please run the Docker command given in README to mount the source directory"
    exit 1
fi

# Build tracker_firmware
cd "${SOURCE_PATH}"
rm -rf build
mkdir -p build
cd build
cmake ..
make -j$(nproc)

echo "tracker_firmware build complete"