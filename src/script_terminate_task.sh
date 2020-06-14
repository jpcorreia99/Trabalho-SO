#!/bin/bash

make clean
make all
./argusd &
./argus -i 0
./argus -m 0
./argus -e "sleep 1" 
./argus -e "sleep 5 | sleep 2 | sleep 5"
./argus -e "sleep 1" 
./argus -t 2
sleep 3
./argus -r
pkill argusd