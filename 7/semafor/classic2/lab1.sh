#!/bin/bash
gcc -c lab1.cpp
gcc -o lab1 lab1.o -lpthread -lrt
./lab1