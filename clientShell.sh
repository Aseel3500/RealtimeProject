#!/bin/bash
gcc -o client client.c

max=10
for (( i=2; i <= $max; ++i ))
do
    ./client $i
done

