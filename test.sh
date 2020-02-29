#!/bin/bash 

for file in FicherosPruebaTokenizador/*
do
    if [[ $file == *".cpp" ]]; then
        cp $file src/main.cpp
        make
        ./practica1 > temp.out
        out_filename="$file.sal"
        diff temp.out $out_filename
        rm temp.out
    fi
done