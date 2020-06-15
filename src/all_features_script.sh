#!/bin/bash

make clean
make all
./argusd &
./argus -e "date" 
./argus -e "ls | wc"
./argus -i 1
./argus -m 3
./argus -e "sleep 4"
./argus -e "sleep 2 | sleep 2"
./argus -m 0
./argus -i 0
./argus -e "sleep 1 | sleep 1"
./argus -e "sleep 2"
./argus -t 5
./argus -e "ls | wc | wc"
sleep 5
./argus -o 7
./argus -r
pkill argusd