#!/bin/bash
make clean
make all
./argusd &
./argus -m 10
./argus -e "ls"
./argus -e "sleep 10"
./argus -e "sleep 10"
./argus -t 2
./argus -r
./argus -o 2
