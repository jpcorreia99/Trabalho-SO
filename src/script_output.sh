#!/bin/bash

make clean
make all
./argusd &
./argus -e "ls | wc" 
sleep 1
./argus -e "date"
sleep 1
./argus -e "sleep 1"
sleep 2
./argus -o 1
./argus -o 2
./argus -o 3
./argus -o 4
pkill argusd

echo "Esperado #1: resultado de ls | wc"
echo "Esperado #2: data atual"
echo "Esperado #3: comando não tem output"
echo "Esperado #4: indice não associado a comando"