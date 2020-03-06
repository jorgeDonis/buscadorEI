#!/bin/bash 

cp ./src/test_speed.cpp ./src/main.cpp
make
./practica1
rm -f ./corpus/*.tk