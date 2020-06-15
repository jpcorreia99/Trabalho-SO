#!/bin/bash

make clean
make all
./argusd &
./argus -i 0
./argus -m 0
./argus -e "sleep 1" 
./argus -e "sleep 5 | sleep 2 | sleep 5"
./argus -t 2
./argus -e "sleep 1" 
./argus -e "ls | wc"
sleep 1
./argus -t 4
./argus -m 1 
./argus -e "sleep 2 | sleep 2"
sleep 3
./argus -t 5
./argus -e "sleep 1" 
./argus -t 6

./argus -r
echo "Esperado #1: terminado normalmente"
echo "Esperado #2: terminada pelo utilizador"
echo "Esperado #3: terminada normalmente"
echo "Esperado #4: terminada normalmente"
echo "Esperado #5: terminada por máximo de execução"
echo "Esperado #6: terminada pelo utilizador"
pkill argusd