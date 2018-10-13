#!/bin/bash
g++ -Ofast kdtree_eval.cpp -std=c++11; 
./a.out $1
# python sample.py $1