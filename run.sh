#!/bin/bash

export ENGINE_ROOT=~/programs/c-c++/engine
cd build
make -j8
./app
cd ..
