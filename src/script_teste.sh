#!/bin/bash

make clean
make all
./argusd &
./argus -e date 
sleep 1
./argus -r 
./argus -o 1
pkill argusd