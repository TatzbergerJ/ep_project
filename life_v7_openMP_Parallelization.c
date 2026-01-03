#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <omp.h>

#define CELL(g, y, x) g[(y) * stride + (x)] 

uint8_t *allocate_grid(int n, int stride) {
    return (uint8_t *)calloc(stride * stride, sizeof(uint8_t)); 
}

void init_grid(uint8_t *grid, int n, int stride) {
    for (int y = 1; y <= n; y++) {
        for (int x = 1; x <= n; x++) {
            CELL(grid, y, x) = rand() % 2;
        }
    }
}

static inline int count_neighbors(uint8_t *grid, int n, int stride, int x, int y) {
    uint8_t *row_above = &CELL(grid, y - 1, x - 1);
    uint8_t *row_curr  = &CELL(grid, y, x - 1);
    uint8_t *row_below = &CELL(grid, y + 1, x - 1);

    int neighbors = 0;
    neighbors += row_above[0] + row_above[1] + row_above[2];
    neighbors += row_curr[0]               + row_curr[2];
    neighbors += row_below[0] + row_below[1] + row_below[2];

    return neighbors;
}



void step(uint8_t *current, uint8_t *next, int n, int stride) {
    #pragma omp parallel for schedule(static)
    for (int y = 1; y <= n; y++) {
        for (int x = 1; x <= n; x++) {
            int neighbors = count_neighbors(current, n, stride, x, y);
            int alive = CELL(current, y, x);

            if (alive)
                CELL(next, y, x) = (neighbors == 2 || neighbors == 3);
            else
                CELL(next, y, x) = (neighbors == 3);
        }
    }
}


int main(int argc, char *argv[]) {
    int n = atoi(argv[1]);
    int generations = atoi(argv[2]);
    unsigned int seed = atoi(argv[3]);

    srand(seed);

    int stride = n + 2;

    uint8_t *grid_a = allocate_grid(n, stride);
    uint8_t *grid_b = allocate_grid(n, stride);

    init_grid(grid_a, n, stride);

    clock_t start = clock();

    for (int gen = 0; gen < generations; gen++) {
        step(grid_a, grid_b, n, stride);

        uint8_t *tmp = grid_a;
        grid_a = grid_b;
        grid_b = tmp;
    }

    clock_t end = clock();
    printf("Time: %.3f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    free(grid_a);
    free(grid_b);

    return 0;
}




