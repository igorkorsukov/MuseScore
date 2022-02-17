#!/usr/bin/env bash

export PATH=/home/igor/build_tools/qt598/bin:$PATH 
export LD_LIBRARY_PATH=/home/igor/build_tools/qt598/lib:$LD_LIBRARY_PATH

rm -rf ./build.release 
make portable 