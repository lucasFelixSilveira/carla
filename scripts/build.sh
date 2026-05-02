#!/bin/bash

cd compiler

# flag -fsanitize=address
g++ -std=c++17 -g -O0 -fPIC -fpermissive -fexceptions main.cpp \
    -o ../build/carla \
    -I.
cd ..
