#!/bin/bash

# Usage: ./benchmark_life.sh PROGRAM N GENERATIONS SEED [print]

if [[ $# -lt 4 || $# -gt 5 ]]; then
    echo "Usage: $0 PROGRAM N GENERATIONS SEED [print]"
    exit 1
fi

PROGRAM=$1
N=$2
GEN=$3
SEED=$4
PRINT=${5:-0}  # default to 0 if not provided

# Check if program exists and is executable
if [[ ! -x $PROGRAM ]]; then
    echo "Error: $PROGRAM not found or not executable."
    exit 1
fi

echo "Running $PROGRAM with N=$N, GENERATIONS=$GEN, SEED=$SEED, PRINT=$PRINT"
echo

# Run perf with hardware counters + /usr/bin/time
perf stat -e cycles,instructions,branch-misses,L1-dcache-loads,L1-dcache-stores,L1-dcache-load-misses,L1-dcache-store-misses,LLC-loads,LLC-stores,LLC-load-misses,LLC-store-misses \
/usr/bin/time -f "\nTime (user): %U s\nTime (system): %S s\nTime (total): %e s" \
$PROGRAM $N $GEN $SEED $PRINT
