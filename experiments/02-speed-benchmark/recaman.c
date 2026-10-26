// Recaman sequence, speed benchmark version (no plotting).
// Build:  gcc -O3 -march=native -o experiments/02-speed-benchmark/recaman experiments/02-speed-benchmark/recaman.c
// Run:    experiments/02-speed-benchmark/recaman 1000000        (Linux/macOS: ./experiments/02-speed-benchmark/recaman 1000000)
//
// Computes a(0)..a(N), prints a(N), the largest value, and the time taken.
// Visited numbers are stored in a bitset (1 bit per integer).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

static uint64_t *bits;      // bit v is 1 if value v has been visited
static uint64_t cap_bits;   // how many values the bitset can hold

static void grow(uint64_t need) {
    uint64_t new_cap = cap_bits;
    while (new_cap <= need) new_cap *= 2;
    uint64_t old_words = cap_bits / 64, new_words = new_cap / 64;
    bits = realloc(bits, new_words * sizeof(uint64_t));
    if (!bits) { fprintf(stderr, "out of memory\n"); exit(1); }
    memset(bits + old_words, 0, (new_words - old_words) * sizeof(uint64_t));
    cap_bits = new_cap;
}

int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s N\n", argv[0]); return 1; }
    uint64_t N = strtoull(argv[1], NULL, 10);

    cap_bits = 64;
    while (cap_bits < 4 * N) cap_bits *= 2;           // start near the expected size
    bits = calloc(cap_bits / 64, sizeof(uint64_t));
    if (!bits) { fprintf(stderr, "out of memory\n"); return 1; }

    clock_t t0 = clock();

    uint64_t cur = 0, max = 0;
    bits[0] |= 1;
    for (uint64_t n = 1; n <= N; n++) {
        if (cur > n && !((bits[(cur - n) >> 6] >> ((cur - n) & 63)) & 1)) {
            cur -= n;                                  // backward jump
        } else {
            cur += n;                                  // forward jump
            if (cur >= cap_bits) grow(cur);
            if (cur > max) max = cur;
        }
        bits[cur >> 6] |= 1ULL << (cur & 63);
    }

    double secs = (double)(clock() - t0) / CLOCKS_PER_SEC;
    printf("N=%llu  a(N)=%llu  max=%llu  time=%.3fs  bitset=%.1f MB\n",
           (unsigned long long)N, (unsigned long long)cur, (unsigned long long)max,
           secs, cap_bits / 8.0 / 1e6);
    free(bits);
    return 0;
}
