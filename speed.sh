#!/bin/bash 

cp ./src/test_speed.cpp ./src/main.cpp
make
time ./practica1
rm ./corpus/*.tk