# Conway’s Game of Life

## Algorithm Description

Conway’s Game of Life is a cellular automaton played on a two-dimensional grid of cells. Each cell exists in one of two states: alive or dead. The grid evolves over a sequence of discrete time steps known as generations.

At each generation, the state of every cell is updated simultaneously based on the number of live neighboring cells it has. Neighbors are defined as the eight surrounding cells (horizontal, vertical, and diagonal).

The update rules are as follows:

- A live cell with fewer than two live neighbors dies (underpopulation).
- A live cell with two or three live neighbors survives.
- A live cell with more than three live neighbors dies (overpopulation).
- A dead cell with exactly three live neighbors becomes alive (reproduction).

By repeatedly applying these rules, simple initial configurations can evolve into complex and emergent patterns over time.

---

## Program Usage

This program simulates Conway’s Game of Life on an N × N grid for a fixed number of generations. The initial grid configuration is generated randomly using a user-specified seed to ensure reproducible results.

### Compilation
Use this for basic compilation (no compiler optimizations):

```
gcc -O0 life.c -o life
```

### Running the Program

```
./life N GENERATIONS SEED [print]
```

### Arguments

- N: Size of the grid (N × N)
- GENERATIONS: Number of generations to simulate
- SEED: Random seed used to initialize the grid
- print (optional):
  - 1 enables printing of the initial and final grid states
  - 0 or omitted disables printing (recommended for benchmarking)

### Examples

Print the initial and final grid states for correctness checking:
```
./life 10 5 42 1
```
Run the simulation without printing output (suitable for performance measurements):
```
./life 500 1000 42
```

# Benchmarking Tool

## Description
This benchmarking script measures the performance of a 'Conway's Game of Life implementation'. 

### Usage
```
./benchmark.sh PROGRAM N GENERATIONS SEED [print]
```

### Arguments:
- PROGRAM: Path to the executable program to benchmark
- N: Size of the grid (N x N)
- GENERATIONS: Number of generations to simulate
- SEED: Random seed used to initialize the grid
- print (optional):
  - 1 enables printing (not recommended for benchmarking!)
  - 0 disables printing (default, recommended for benchmarking!)

### Example:
```
./benchmark.sh ./life 500 1000 42
```

### Output:
The script outputs the following performance metrics:
- cycles
- instructions
- branch misses
- L1 cache loads/stores + misses
- last-level cache (LLC) loads/stores + misses
- elapsed time (user, system & total)
