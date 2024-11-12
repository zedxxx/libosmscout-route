#!/usr/bin/bash -ex

bin="bin"

cmake -B ${bin} -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

cd ${bin}

mingw32-make -j4
