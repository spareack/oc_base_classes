#!/bin/bash
g++ -c client.cpp
g++ -o client client.o -lpthread -lrt
./client