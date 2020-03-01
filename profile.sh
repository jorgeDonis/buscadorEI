#!/bin/bash 

sudo perf record -g -- ./practica1
sudo perf script | c++filt | python gprof2dot.py -f perf | dot -Tpng -o profile.png