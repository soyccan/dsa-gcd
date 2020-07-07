#!/bin/sh
while true; do
    python hw1_6-test.py >logpy
    ./hw1_6 < hw1_6.in > out 2>log
    diff out hw1_6.out
    if [ $? != 0 ]; then
        break
    fi
    printf .
done