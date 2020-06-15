#!/bin/bash

make clean
make all
./argusd &
./argus -o 1
echo "Expected: no command"
./argus -m 0
./argus -i 0
./argus -e "sleep 5"
sleep 1
./argus -t 1
sleep 1
./argus -o 1
echo "Expected: no output"
./argus -o 2
echo "Expected: no command"
./argus -m 1
./argus -e "sleep 2"
sleep 3
./argus -o 2
echo "Expected: no output"
./argus -m 2
sleep 1
./argus -e "ls | sleep 1 | wc"
sleep 2
./argus -o 3
echo "Expected: 0 | 0 | 0"
./argus -e "ls | sleep 1 | wc"
./argus -t 4
sleep 1
./argus -o 4
echo "Expected: no output"
./argus -r
pkill argusd



