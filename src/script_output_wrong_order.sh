#!/bin/bash

make clean
make all

./argusd &
./argus -e "ls | wc" 
sleep 1
./argus -e "sleep 40"
./argus -e "date"
sleep 1
./argus -o 2
./argus -o 3
pkill argusd

echo "Esperado #2: comando nao tem output"
echo "Esperado #3: data atual"
