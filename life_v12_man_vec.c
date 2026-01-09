#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define CELL(g, y, x) g[(y) * stride + (x)]
#if defined(__AVX512F__)
typedef unsigned char vu8 __attribute__((vector_size(64)));
#define VBYTES 64
#elif defined(__AVX2__)
typedef unsigned char vu8 __attribute__((vector_size(32)));
#define VBYTES 32
#else
typedef unsigned char vu8 __attribute__((vector_size(16)));
#define VBYTES 16
#endif

static inline vu8 load_vu8(const uint8_t *p) {
    vu8 v;
    __builtin_memcpy(&v, p, sizeof(v));   // safe even if p is unaligned
    return v;
}

static inline void store_vu8(uint8_t *p, vu8 v) {
    __builtin_memcpy(p, &v, sizeof(v));
}
uint8_t *allocate_grid(int n, int stride) {
    return (uint8_t *)calloc(stride * stride, sizeof(uint8_t));
}
static uint64_t hash_grid(const uint8_t *grid, size_t n, size_t stride)
{
    uint64_t h = 1469598103934665603ULL;
    for (size_t y = 1; y <= n; y++) {
        const uint8_t *row = grid + y * stride;
        for (size_t x = 1; x <= n; x++) {
            h ^= row[x];
            h *= 1099511628211ULL;
        }
    }
    return h;
}
static void print_grid(const uint8_t *grid, size_t n, size_t stride)
{
    for (size_t y = 1; y <= n; y++) {
        const uint8_t *row = grid + y * stride;
        for (size_t x = 1; x <= n; x++) {
            putchar(row[x] ? '#' : '.');
        }
        putchar('\n');
    }
}

void init_grid(uint8_t *grid, int n, int stride) {
    for (int y = 1; y <= n; y++) {
        for (int x = 1; x <= n; x++) {
            CELL(grid, y, x) = rand() % 2;
        }
    }
}

static inline int count_neighbors(uint8_t* restrict grid, int n, int stride, int x, int y) {
    uint8_t *row_above = &CELL(grid, y - 1, x - 1);
    uint8_t *row_curr  = &CELL(grid, y, x - 1);
    uint8_t *row_below = &CELL(grid, y + 1, x - 1);

    int neighbors = 0;
    neighbors += row_above[0] + row_above[1] + row_above[2];
    neighbors += row_curr[0]               + row_curr[2];
    neighbors += row_below[0] + row_below[1] + row_below[2];

    return neighbors;
}



void step(uint8_t* restrict current, uint8_t* restrict next, int n, int stride) {
    #pragma omp parallel for schedule(static)
    for (int y = 1; y <= n; y++) {
        const uint8_t *above = current + (y - 1) * stride;
        const uint8_t *cur   = current +  y      * stride;
        const uint8_t *below = current + (y + 1) * stride;
        uint8_t *out         = next    +  y      * stride;

        int x = 1;

        // SIMD part: process VBYTES cells per iteration
        // Needs x-1 .. x+VBYTES in-bounds; padding (halo) makes this safe.
        for (; x <= n - (VBYTES - 1); x += VBYTES) {
            vu8 aL = load_vu8(above + x - 1);
            vu8 aC = load_vu8(above + x);
            vu8 aR = load_vu8(above + x + 1);

            vu8 cL = load_vu8(cur + x - 1);
            vu8 cR = load_vu8(cur + x + 1);

            vu8 bL = load_vu8(below + x - 1);
            vu8 bC = load_vu8(below + x);
            vu8 bR = load_vu8(below + x + 1);

            vu8 alive = load_vu8(cur + x);          // 0/1 per lane

            // neighbors = sum of 8 surrounding cells (0..8 fits in u8)
            vu8 neigh = aL + aC + aR
                      + cL      + cR
                      + bL + bC + bR;

            // Comparisons produce 0xFF for true, 0x00 for false (per lane)
            vu8 is3 = (neigh == (vu8){3});
            vu8 is2 = (neigh == (vu8){2});

            // Convert alive (0/1) -> mask (0x00/0xFF) by comparing to 0
            vu8 alive_mask = (alive != (vu8){0});

            // result mask: (neighbors==3) | (alive & neighbors==2)
            vu8 res_mask = is3 | (alive_mask & is2);

            // Convert 0x00/0xFF -> 0/1
            vu8 res = res_mask & (vu8){1};

            store_vu8(out + x, res);
        }

        // Scalar tail (remaining cells)
        for (; x <= n; x++) {
            int neighbors =
                above[x - 1] + above[x] + above[x + 1] +
                cur  [x - 1]            + cur  [x + 1] +
                below[x - 1] + below[x] + below[x + 1];

            uint8_t alive_s = cur[x];
            out[x] = (uint8_t)((neighbors == 3) | (alive_s & (neighbors == 2)));
        }
    }
}


int main(int argc, char *argv[]) {
    int n = atoi(argv[1]);
    int generations = atoi(argv[2]);
    unsigned int seed = atoi(argv[3]);
    int print_mode  = atoi(argv[4]);

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
    //printf("Time: %.3f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);
    if (print_mode == 1) {
        print_grid(grid_a, n, stride);
    } else if (print_mode == 2) {
        uint64_t h = hash_grid(grid_a, n, stride);
        printf("HASH 0x%016llx\n",
               (unsigned long long)h);
    }
    free(grid_a);
    free(grid_b);

    return 0;
}




