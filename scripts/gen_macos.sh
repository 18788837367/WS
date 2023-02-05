#!/bin/sh

CURRENT_DIR=$(pwd)
ROOT_DIR=${CURRENT_DIR}/..
BUILD_DIR=${ROOT_DIR}/build
CMAKE_DIR=${ROOT_DIR}/cmake
BIN_DIR=${ROOT_DIR}/bin
SCRIPT_DIR=${ROOT_DIR}/script

if [ ! -d ${BUILD_DIR} ]; then
    mkdir ${BUILD_DIR}
fi

cmake -G"Xcode" -DCMAKE_TOOLCHAIN_FILE=${CMAKE_DIR}/macos.xcode.toolchain.cmake -S ${ROOT_DIR} -B ${BUILD_DIR}
cmake --build ${BUILD_DIR}   
