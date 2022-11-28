#!/bin/bash
g++ -c parent.cpp
g++ -o parent parent.o -lpthread
sudo setcap CAP_SYS_TIME=pe parent
sudo ./parent