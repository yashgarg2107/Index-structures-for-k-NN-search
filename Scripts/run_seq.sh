#!/bin/bash
g++ -Ofast seq_eval.cpp -std=c++11; 
./a.out $1
# python sample.py $1