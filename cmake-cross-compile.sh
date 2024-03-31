#!/bin/bash

cd "$PWD"/out/build
cmake -DCMAKE_TOOLCHAIN_FILE=../../tc-windows.cmake ../../
