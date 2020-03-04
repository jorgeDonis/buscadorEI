#!/bin/bash 

for file in FicherosPruebaTokenizador/*
do
    if [[ $file == *".cpp" ]]; then
        cp $file src/main.cpp
        echo [Testing] $file
        make
        ./practica1 > temp.out
        out_filename="$file.sal"
        diff temp.out $out_filename
        rm temp.out
    fi
done
