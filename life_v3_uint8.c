#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define CELL(g, y, x) g[(y) * n + (x)]

uint8_t *allocate_grid(int n) {
    return (uint8_t *)malloc(n * n * sizeof(uint8_t));
}

void init_grid(uint8_t *grid, int n) {
    for (int i = 0; i < n * n; i++)
        grid[i] = rand() % 2;
}

int count_neighbors(uint8_t *grid, int n, int x, int y) {
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0)
                continue;

            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < n && ny >= 0 && ny < n)
                count += CELL(grid, ny, nx);
        }
    }

    return count;
}


void step(uint8_t *current, uint8_t *next, int n) {
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < n; x++) {
            int neighbors = count_neighbors(current, n, x, y);
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
    unsigned int seed = (unsigned int)atoi(argv[3]);



    srand(seed);

    uint8_t *grid_a = allocate_grid(n);
    uint8_t *grid_b = allocate_grid(n);

    init_grid(grid_a, n);

 
    clock_t start = clock();

    for (int gen = 0; gen < generations; gen++) {
        step(grid_a, grid_b, n);

        uint8_t *tmp = grid_a;
        grid_a = grid_b;
        grid_b = tmp;
    }


    clock_t end = clock();
    printf("Time: %.3f seconds\n",
           (double)(end - start) / CLOCKS_PER_SEC);


    free(grid_a);
    free(grid_b);

    return 0;
}


