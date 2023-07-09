#!/bin/bash

ARM64_GCC_DIR=/opt/gcc-aarch64/
ARM_GCC_DIR=/opt/gcc-arm/

GCC_DIR=
MINGW_GCC_DIR=

#设置平台参数
if [ $1 == "aarch64" ]
then
    echo "set aarch64 env"
    export PLATFORM=aarch64-linux-gnu
    export ARCH=aarch64
    export PATH=${PATH}:${ARM64_GCC_DIR}/bin

elif [ $1 == "arm" ]
then
    echo "set arm env"
    export PLATFORM=arm-linux-gnueabihf
    export ARCH=arm
    export PATH=${PATH}:${ARM_GCC_DIR}/bin
else
    echo "set default env"
fi




