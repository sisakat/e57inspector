#!/bin/bash

sudo apt update
sudo apt install -y build-essential qt6-base-dev libxerces-c-dev
cmake -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build build --config $BUILD_TYPE
