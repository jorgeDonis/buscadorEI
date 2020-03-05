#!/bin/bash 

cp ./src/test_speed.cpp ./src/main.cpp
make
./practica1


for file in corpus/*
do
    if [[ $file == *".txt" ]]; then
        file_size=$(ls -l  $file | cut -d " " -f5)
        token_size=$(ls -l $file.tk | cut -d " " -f5)
        if [ "$file_size" -ge "$token_size" ]; then
            pctg=`echo "(($file_size - $token_size) / $file_size) * 100" | bc -l`
            echo Tamaño de $file = [$file_size] vs [$token_size] $pctg
        fi
    fi
done

rm -f corpus/*.tk
