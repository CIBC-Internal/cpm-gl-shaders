#!/bin/bash
cd "$(dirname "$0")"

if [ ! -d ./bin ]; then
  mkdir -p ./bin
fi

# Ensure we fail immediately if any command fails.
set -e

pushd ./bin > /dev/null
  if [[ ! "-n" == $1 ]]; then
    cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_OS_MESA=ON -D CMAKE_EXPORT_COMPILE_COMMANDS="ON" ..
  fi
  make -j4
  ./gl_shader_tests
popd

PROJECT_BASE_DIR=${HOME}/me/games/leaf
PROJECT_BIN_DIR=${PROJECT_BASE_DIR}/build/bin

source ${HOME}/prosp/common/common.sh
MergeCompileCommands $PROJECT_BIN_DIR
ConstructYCMFile $PROJECT_BASE_DIR $PROJECT_BIN_DIR
