#!/bin/bash

make clean
make all
./argusd &
./argus -e "ls | wc" 
sleep 1
./argus -o 1
pkill argusd