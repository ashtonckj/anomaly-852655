// Low-RAM pattern scanner for the Recaman sequence (experiment 03).
// Build:  gcc -O3 -march=native -o experiments/03-patterns/scan experiments/03-patterns/scan.c
// Run:    ./experiments/03-patterns/scan 1000000000 experiments/03-patterns/runs.csv experiments/03-patterns/checkpoints.csv
//
// runs.csv         every long zigzag run:  n_start,length,lo,hi,fwd_parity
//                  ("long" = length >= max(2000, n_start/1000))
// checkpoints.csv  at N = 1,2,5 x 10^k:  N, max, smallest_unvisited, unvisited_below_max, mem_MB
//
// The number line is cut into chunks of 4096 values:
//   untouched chunk -> no memory, solid chunk -> no memory, mixed chunk -> a 512-byte bitmap.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;

#define CHUNK_BITS  12
#define CHUNK_SIZE  (1u << CHUNK_BITS)
#define CHUNK_WORDS (CHUNK_SIZE / 64)
#define SEG_BITS    16
#define SEG_SLOTS   (1u << SEG_BITS)
#define MAX_SEGS    (1u << 16)
#define EMPTY       0u
#define FULL        0xFFFFFFFFu

static u32 *tab;
static u64  tab_len;
static u64 *seg_data[MAX_SEGS];
static u16 *seg_cnt[MAX_SEGS];
static u32  segs_used, next_new_slot, free_head;
static u64  visited;

#define SLOT_DATA(s) (seg_data[((s) - 1) >> SEG_BITS] + (u64)(((s) - 1) & (SEG_SLOTS - 1)) * CHUNK_WORDS)
#define SLOT_CNT(s)  (seg_cnt [((s) - 1) >> SEG_BITS][((s) - 1) & (SEG_SLOTS - 1)])

static void die(void) { fprintf(stderr, "out of memory\n"); exit(1); }

static void tab_grow(u64 need) {
    u64 n = tab_len; while (n <= need) n *= 2;
    tab = realloc(tab, n * sizeof(u32)); if (!tab) die();
    memset(tab + tab_len, 0, (n - tab_len) * sizeof(u32));
    tab_len = n;
}

static u32 alloc_slot(void) {
    if (free_head) {
        u32 s = free_head; u64 *d = SLOT_DATA(s);
        free_head = (u32)d[0];
        memset(d, 0, CHUNK_WORDS * 8); SLOT_CNT(s) = 0;
        return s;
    }
    if (next_new_slot == segs_used * SEG_SLOTS) {
        if (segs_used == MAX_SEGS) die();
        seg_data[segs_used] = calloc((u64)SEG_SLOTS * CHUNK_WORDS, 8);
        seg_cnt[segs_used]  = calloc(SEG_SLOTS, sizeof(u16));
        if (!seg_data[segs_used] || !seg_cnt[segs_used]) die();
        segs_used++;
    }
    return ++next_new_slot;
}

static inline int test(u64 v) {
    u32 s = tab[v >> CHUNK_BITS];
    if (s == EMPTY) return 0;
    if (s == FULL)  return 1;
    u64 o = v & (CHUNK_SIZE - 1);
    return (SLOT_DATA(s)[o >> 6] >> (o & 63)) & 1;
}

static inline void mark(u64 v) {
    u64 c = v >> CHUNK_BITS;
    if (c >= tab_len) tab_grow(c);
    u32 s = tab[c];
    if (s == FULL) return;
    if (s == EMPTY) { s = alloc_slot(); tab[c] = s; }
    u64 *d = SLOT_DATA(s), o = v & (CHUNK_SIZE - 1), bit = 1ULL << (o & 63);
    if (d[o >> 6] & bit) return;
    d[o >> 6] |= bit; visited++;
    if (++SLOT_CNT(s) == CHUNK_SIZE) { d[0] = free_head; free_head = s; tab[c] = FULL; }
}

static double mem_mb(void) {
    return ((double)segs_used * ((double)SEG_SLOTS * CHUNK_WORDS * 8 + SEG_SLOTS * 2) + (double)tab_len * 4) / 1e6;
}

int main(int argc, char **argv) {
    if (argc != 4) { fprintf(stderr, "usage: %s N runs.csv checkpoints.csv\n", argv[0]); return 1; }
    u64 N = strtoull(argv[1], 0, 10);
    FILE *fr = fopen(argv[2], "w"), *fc = fopen(argv[3], "w");
    if (!fr || !fc) { fprintf(stderr, "cannot open output files\n"); return 1; }
    fprintf(fr, "n_start,length,lo,hi,fwd_parity\n");
    fprintf(fc, "N,max,smallest_unvisited,unvisited_below_max,mem_MB\n");

    tab_len = 1u << 16; tab = calloc(tab_len, sizeof(u32)); if (!tab) die();

    u64 cur = 0, max = 0, lowest = 1;
    mark(0);
    int prev_back = -1, rdir0 = 0; u64 rs = 1, rlen = 0, rlo = 0, rhi = 0;
    u64 next_cp = 1; clock_t t0 = clock();

    for (u64 n = 1; n <= N; n++) {
        int back = (cur > n && !test(cur - n));
        cur = back ? cur - n : cur + n;
        if (cur > max) max = cur;
        mark(cur);

        if (prev_back != -1 && back != prev_back) {
            rlen++;
            if (cur < rlo) rlo = cur;
            if (cur > rhi) rhi = cur;
        } else {
            u64 thr = rs / 1000 > 2000 ? rs / 1000 : 2000;
            if (rlen >= thr)
                fprintf(fr, "%llu,%llu,%llu,%llu,%d\n", (unsigned long long)rs, (unsigned long long)rlen,
                        (unsigned long long)rlo, (unsigned long long)rhi, (int)(rdir0 ? (rs + 1) & 1 : rs & 1));
            rs = n; rlen = 1; rlo = rhi = cur; rdir0 = back;
            u64 before = back ? cur + n : cur - n;
            if (before < rlo) rlo = before;
            if (before > rhi) rhi = before;
        }
        prev_back = back;

        if (n == next_cp) {
            while (lowest <= max && test(lowest)) lowest++;
            fprintf(fc, "%llu,%llu,%llu,%llu,%.1f\n", (unsigned long long)n, (unsigned long long)max,
                    (unsigned long long)lowest, (unsigned long long)(max + 1 - visited), mem_mb());
            u64 p = 1; while (p * 10 <= n) p *= 10;
            next_cp = (n / p == 1) ? 2 * p : (n / p == 2) ? 5 * p : 10 * p;
        }
        if (n % 1000000000ULL == 0)
            fprintf(stderr, "n=%.0e  max=%llu  mem=%.0f MB  %.0f s\n", (double)n, (unsigned long long)max,
                    mem_mb(), (double)(clock() - t0) / CLOCKS_PER_SEC);
    }
    fclose(fr); fclose(fc);
    u64 full = 0; for (u64 c = 0; c <= (max >> CHUNK_BITS); c++) full += (tab[c] == FULL);
    printf("done N=%llu max=%llu  distinct=%llu  solid chunks=%llu  mixed chunks (peak)=%llu  memory=%.0f MB\n",
           (unsigned long long)N, (unsigned long long)max, (unsigned long long)visited,
           (unsigned long long)full, (unsigned long long)next_new_slot, mem_mb());
    return 0;
}