#!/bin/bash

cd compiler

g++ -std=c++17 -g -O0 -fPIC -fpermissive -fexceptions main.cpp \
    -o ../build/carla \
    -I. \
    `llvm-config --cxxflags --ldflags --libs core support irreader`

cd ..
