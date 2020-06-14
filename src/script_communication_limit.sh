#!/bin/bash

make clean
make all
./argusd &
./argus -i 1 
./argus -e "sleep 2 | sleep 2"
./argus -i 3 
./argus -e "sleep 2 | sleep 2"
sleep 3
./argus -r
pkill argusd