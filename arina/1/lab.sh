#!/bin/bash
gcc -c lab.cpp
gcc -o lab lab.o -lpthread -lrt
./lab