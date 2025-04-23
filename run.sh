#!/bin/bash

case=./case/case3.txt

make
./a.out $case 0
python3 ./visulization/visualize.py

