#!/bin/bash

make clean
make all
./argusd &
./argus -o 1
./argus -m 0
./argus -i 0
./argus -e "sleep 5"
sleep 1
./argus -t 1
sleep 1
./argus -o 1
./argus -o 2
./argus -m 1
./argus -e "sleep 2"
sleep 3
./argus -o 2
./argus -m 2
sleep 1
./argus -e "ls | sleep 1 | wc"
sleep 1
./argus -o 3
./argus -e "ls | sleep 1 | wc"
./argus -t 4
./argus -r
./argus -o 4


