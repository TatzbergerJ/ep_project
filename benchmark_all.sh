#!/bin/bash

# Performance benchmarking script for multiple program versions
# Usage: ./benchmark.sh <param1> <param2> <param3>
# Example: ./benchmark.sh 1000 10 0

# Check if required parameters are provided
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <param1> <param2> <param3>"
    echo "Example: $0 1000 10 0"
    exit 1
fi

# Parameters for the programs
PARAM1=$1
PARAM2=$2
PARAM3=$3

# Configuration
ITERATIONS=5
PROGRAMS=("life" "life_v1" "life_v2" "life_v3" "life_v4" "life_v5" "life_v6" "life_v7" "life_v8" "life_v9" "life_v10" "life_v11" "life_v12")
PERF_EVENTS="cycles,instructions,branch-misses,L1-dcache-loads,L1-dcache-stores,L1-dcache-load-misses,L1-dcache-store-misses,LLC-loads,LLC-stores,LLC-load-misses,LLC-store-misses"
OUTPUT_FILE="benchmark_results_${PARAM1}_${PARAM2}_${PARAM3}.csv"
TEMP_DIR="perf_temp_$$"

# Create temporary directory for intermediate results
mkdir -p "$TEMP_DIR"

echo "Starting benchmark with parameters: $PARAM1 $PARAM2 $PARAM3"
echo "Running $ITERATIONS iterations for ${#PROGRAMS[@]} programs"
echo "Output will be saved to: $OUTPUT_FILE"
echo ""

# Create CSV header
echo "program,iteration,cycles,instructions,branch-misses,L1-dcache-loads,L1-dcache-stores,L1-dcache-load-misses,L1-dcache-store-misses,LLC-loads,LLC-stores,LLC-load-misses,LLC-store-misses,elapsed_time,user_time,sys_time" > "$OUTPUT_FILE"

# Run benchmarks in round-robin fashion
for iter in $(seq 1 $ITERATIONS); do
    echo "=== Iteration $iter/$ITERATIONS ==="
    
    for prog in "${PROGRAMS[@]}"; do
        # Check if program exists
        if [ ! -x "./$prog" ]; then
            echo "Warning: $prog not found or not executable, skipping..."
            continue
        fi
        
        echo "  Running $prog..."
        
        # Run perf stat and capture output
        PERF_OUTPUT="$TEMP_DIR/${prog}_${iter}.txt"
        perf stat -e "$PERF_EVENTS" -o "$PERF_OUTPUT" "./$prog" "$PARAM1" "$PARAM2" "$PARAM3" 0 2>&1 > /dev/null
        
        # Parse perf output and extract metrics
        cycles=$(grep -oP '[\d,]+(?=\s+cycles)' "$PERF_OUTPUT" | tr -d ',')
        instructions=$(grep -oP '[\d,]+(?=\s+instructions)' "$PERF_OUTPUT" | tr -d ',')
        branch_misses=$(grep -oP '[\d,]+(?=\s+branch-misses)' "$PERF_OUTPUT" | tr -d ',')
        l1_dcache_loads=$(grep -oP '[\d,]+(?=\s+L1-dcache-loads)' "$PERF_OUTPUT" | tr -d ',')
        l1_dcache_stores=$(grep -oP '[\d,]+(?=\s+L1-dcache-stores)' "$PERF_OUTPUT" | tr -d ',')
        l1_dcache_load_misses=$(grep -oP '[\d,]+(?=\s+L1-dcache-load-misses)' "$PERF_OUTPUT" | tr -d ',')
        l1_dcache_store_misses=$(grep -oP '[\d,]+(?=\s+L1-dcache-store-misses)' "$PERF_OUTPUT" | tr -d ',')
        llc_loads=$(grep -oP '[\d,]+(?=\s+LLC-loads)' "$PERF_OUTPUT" | tr -d ',')
        llc_stores=$(grep -oP '[\d,]+(?=\s+LLC-stores)' "$PERF_OUTPUT" | tr -d ',')
        llc_load_misses=$(grep -oP '[\d,]+(?=\s+LLC-load-misses)' "$PERF_OUTPUT" | tr -d ',')
        llc_store_misses=$(grep -oP '[\d,]+(?=\s+LLC-store-misses)' "$PERF_OUTPUT" | tr -d ',')
        elapsed_time=$(grep -oP '[\d.]+(?=\s+seconds time elapsed)' "$PERF_OUTPUT")
        user_time=$(grep -oP '[\d.]+(?=\s+seconds user)' "$PERF_OUTPUT")
        sys_time=$(grep -oP '[\d.]+(?=\s+seconds sys)' "$PERF_OUTPUT")
        
        # Write to CSV
        echo "$prog,$iter,$cycles,$instructions,$branch_misses,$l1_dcache_loads,$l1_dcache_stores,$l1_dcache_load_misses,$l1_dcache_store_misses,$llc_loads,$llc_stores,$llc_load_misses,$llc_store_misses,$elapsed_time,$user_time,$sys_time" >> "$OUTPUT_FILE"
    done
    
    echo ""
done

# Cleanup temporary directory
rm -rf "$TEMP_DIR"

echo "Benchmark complete! Results saved to: $OUTPUT_FILE"
echo ""
echo "You can now analyze the results using tools like:"
echo "  - Python/pandas"
echo "  - R"
echo "  - Excel/LibreOffice Calc"
echo "  - Or any CSV-compatible tool"
