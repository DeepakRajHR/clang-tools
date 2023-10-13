#!/bin/bash
#
set -x

LLVM_INSTALL_DIR=/usr/local/opt/llvm@16
LLVM_CONFIG_OPTS="--cxxflags --ldflags --libs analysis --system-libs"

if [ -d build ]; then
  rm -rf build
fi
mkdir build

${LLVM_INSTALL_DIR}/bin/clang++ \
  -g \
  `${LLVM_INSTALL_DIR}/bin/llvm-config ${LLVM_CONFIG_OPTS}` \
  -stdlib=libc++ \
  -lclang-cpp \
  refactor.cpp \
  -o build/refactor

#EOF
