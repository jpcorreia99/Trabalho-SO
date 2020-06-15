#!/bin/bash

make clean
make all
./argusd &
./argus -i 1 
sleep 1
./argus -e "sleep 2 | sleep 2"
./argus -i 3 
sleep 1
./argus -e "sleep 1 | sleep 1"
./argus -i 1
sleep 1
./argus -e "sleep 2 | sleep 2"
./argus -i 0
sleep 1
./argus -e "sleep 2 | sleep 2"
sleep 3
./argus -r
pkill argusd

echo "Esperado #1: ultrapassa limite"
echo "Esperado #2: termina normalmente"
echo "Esperado #3: ultrapassa limite"
echo "Esperado #4: termina normalmente"
