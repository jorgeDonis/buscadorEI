#!/bin/bash
for file in ./tads/*.cpp
do
    cp $file ./src/main.cpp
    make
    ./buscador > ./tads/.temp.salida
    colordiff ./tads/.temp.salida "${file/.cpp/.cpp.sal}"
done
