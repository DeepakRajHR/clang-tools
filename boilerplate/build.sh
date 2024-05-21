#!/bin/bash

if [ "$#" -ne "1" ]; then 
  echo "Provide a single source file"
  exit 1
fi

LLVM_INSTALL_DIR=/usr/local/lib/llvm
LLVM_CONFIG_OPTS="--cxxflags --ldflags --libs analysis --system-libs"

if [ -d build ]; then
  rm -rf build
fi
mkdir build

clang++ \
  `${LLVM_INSTALL_DIR}/bin/llvm-config ${LLVM_CONFIG_OPTS}` \
  -lclang-cpp \
  $1 \
  -o build/tool

#EOF
