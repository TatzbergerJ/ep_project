#!/usr/bin/env bash

for file in life_v*_*.c; do
    # extract "life_v<number>"
    exe=$(echo "$file" | sed -E 's/^(life_v[0-9]+).*/\1/')
    gcc -fopenmp -O2 "$file" -o "$exe"
done
