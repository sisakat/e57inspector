#!/bin/bash

sudo apt update
sudo apt install -y \
  build-essential \
  qt6-base-dev \
  qt6-base-dev-tools \
  qt6-tools-dev \
  libgl1-mesa-dev \
  libglu1-mesa-dev \
  libx11-dev \
  libxext-dev \
  libxrender-dev \
  libxcb1-dev \
  libxkbcommon-x11-dev \
  libxi-dev \
  libxerces-c-dev
cmake -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build build --config $BUILD_TYPE
