#! /bin/bash

printf "\n\nCzas dla programu operujacego na watkach:\n"

{ time ./thr.o $1 $2 $3 $4 $5; }

printf "\nCzas dla programu operujacego na procesach:\n"

{ time ./pro.o $1 $2 $3 $4 $5; }

printf "\n"
