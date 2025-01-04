#!/usr/bin/bash

mkdir -p build/
cd build/
cmake ..
cd ../example/
cmake --build ../build/ --target example && ../build/example
