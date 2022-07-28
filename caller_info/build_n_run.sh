#!/bin/bash
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}
# echo $LD_LIBRARY_PATH
g++ -c -o lib/libcaller.o caller.cc -std=c++11  -lpthread -ldl -fPIC
g++ -shared -std=c++11  -o lib/libcaller.so lib/libcaller.o
g++ -L./lib/ -Wall -std=c++11 -o test main.cc -lcaller -ldl 

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/lib
./test