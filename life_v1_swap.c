#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Allocate an N x N grid */
int **allocate_grid(int n) {
    int **grid = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        grid[i] = malloc(n * sizeof(int));
    }
    return grid;
}

/* Free an N x N grid */
void free_grid(int **grid, int n) {
    for (int i = 0; i < n; i++) {
        free(grid[i]);
    }
    free(grid);
}

/* Initialize grid with random 0/1 values */
void init_grid(int **grid, int n) {
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < n; x++) {
            grid[y][x] = rand() % 2;
        }
    }
}

/* Count live neighbors of cell (x, y) */
int count_neighbors(int **grid, int n, int x, int y) {
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0)
                continue;

            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < n &&
                ny >= 0 && ny < n) {
                count += grid[ny][nx];
            }
        }
    }

    return count;
}

/* Compute next generation */
void step(int **current, int **next, int n) {
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < n; x++) {
            int neighbors = count_neighbors(current, n, x, y);

            if (current[y][x]) {
                next[y][x] = (neighbors == 2 || neighbors == 3);
            } else {
                next[y][x] = (neighbors == 3);
            }
        }
    }
}

/* Optional: print grid (only practical for small N) */
void print_grid(int **grid, int n) {
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < n; x++) {
            putchar(grid[y][x] ? '#' : '.');
        }
        putchar('\n');
    }
    putchar('\n');
}

int main(int argc, char *argv[]) {
    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Usage: %s N GENERATIONS SEED [print]\n", argv[0]);
        fprintf(stderr, "  print: 1 = enable output, 0 = disable output (default)\n");
	return 1;
    }

    int n = atoi(argv[1]);
    int generations = atoi(argv[2]);
    unsigned int seed = (unsigned int)atoi(argv[3]);
    int print_enabled = 0;

    if (argc == 5) {
        print_enabled = atoi(argv[4]);
    }


    if (n <= 0 || generations <= 0) {
        fprintf(stderr, "N and GENERATIONS must be positive integers\n");
        return 1;
    }

    srand(seed);

    int **grid_a = allocate_grid(n);
    int **grid_b = allocate_grid(n);

    init_grid(grid_a, n);
    if (print_enabled) {
    	printf("Initial state:\n");
    	print_grid(grid_a, n);
    }

    for (int gen = 0; gen < generations; gen++) {

        step(grid_a, grid_b, n);

        /* POINTER SWAP INSTEAD OF COPY */
        int **tmp = grid_a;
        grid_a = grid_b;
        grid_b = tmp;
    }

    if (print_enabled) {
    	printf("Final state after %d generations:\n", generations);
    	print_grid(grid_a, n);
    }

    free_grid(grid_a, n);
    free_grid(grid_b, n);

    return 0;
}

