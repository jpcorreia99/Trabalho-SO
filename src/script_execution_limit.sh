#!/bin/bash

make clean
make all
./argusd &
./argus -m 1 
./argus -e "sleep 2"
./argus -m 3 
./argus -e "sleep 2"
./argus -m 0 
./argus -e "sleep 2"
sleep 3
./argus -r
echo "Esperado #1: ultrapassa limite"
echo "Esperado #2: termina normalmente"
echo "Esperado #3: termina normalmente"
pkill argusd