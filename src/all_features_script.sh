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


./argus -r

echo "Expected #1: data"
echo "Expected #2: Concluido"
echo "Expected #3: Terminado por excesso de tempo de execução"
echo "Expected #4: Terminado por falta de comunicação"
echo "Expected #5: Terminado pelo utilizador"
echo "Expected #6: Concluido"
echo "Expected #7: Concluido"

./argus -o 7
echo "Expected Output: x | y | z"

pkill argusd
make clean

