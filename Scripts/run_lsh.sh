#!/bin/bash
g++ -Ofast lsh_eval.cpp -std=c++11; 
./a.out $1 $2
# python sample.py $1