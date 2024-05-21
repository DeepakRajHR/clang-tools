#!/bin/bash

set -x

# LLVM_INSTALL_DIR=/usr/local/opt/llvm-15
LLVM_CONFIG_OPTS="--cxxflags --ldflags --libs analysis --system-libs"

if [ -d build ]; then
  rm -rf build
fi
mkdir build

clang++ \
  -g \
  `llvm-config ${LLVM_CONFIG_OPTS}` \
  -lclang-cpp \
  tool.cpp \
  -o build/fcompute

#EOF
