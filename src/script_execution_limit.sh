#!/bin/bash

make clean
make all
./argusd &
./argus -m 1 
./argus -e "sleep 2"
./argus -m 3 
./argus -e "sleep 2"
sleep 3
./argus -r
pkill argusd