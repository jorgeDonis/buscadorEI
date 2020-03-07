#!/bin/bash

cp src/test_tokenizar_ficheros.cpp src/main.cpp
make
export LANG=es_ES.iso88591

while :
do
    clear
    read input
    echo "$input" > corpus.txt
    ./practica1
    clear
    cat corpus.txt.tk
    read input
done