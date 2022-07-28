#!/bin/bash
rm main
g++ main.cc -lpthread -std=c++11 -o main && ./main
