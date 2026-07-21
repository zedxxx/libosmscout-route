#!/usr/bin/bash -ex

build_dir="build"

cmake -S . -B ${build_dir} -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release --fresh

cmake --build ${build_dir} -j

rm -rf ${build_dir}
