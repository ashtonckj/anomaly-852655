# 02: Speed benchmark

[← All experiments](../README.md)

**Status:** planned

## Question

How long does it take to compute $10^6$, $10^9$ (and maybe $10^{10}$) terms of the sequence, with no plotting, and which language is fastest?

## Method

**Task (identical in every language):** compute $a(0) \dots a(N)$, keep only the last value and the largest value, print them. No arrays of terms are stored and no images are made.

**Correctness check.** Every implementation must print the same numbers. Known reference from the Python baseline:

| N | a(N) | largest value |
| --- | --- | --- |
| $10^6$ | 2,057,164 | 5,946,126 |
| $10^9$ | to be filled by the first run | |

**Visited set.** The hot loop is dominated by "have I seen this number?", so use a **bitset** (1 bit per integer) rather than a byte array or hash set. The largest value was about $6\times$ the number of terms in earlier runs, so $10^9$ terms needs roughly $6\times10^9$ bits, about 750 MB (an estimate to confirm). Make sure the machine has the RAM.

**Languages to try:**

| Language | Why | Build |
| --- | --- | --- |
| C | baseline for "as fast as it gets" | `gcc -O3 -march=native` |
| Rust | same speed class, safer | `cargo build --release` |
| Zig | same speed class | `zig build -Doptimize=ReleaseFast` |
| C++ | compare against C | `g++ -O3 -march=native` |
| Python | slow reference point | plain `python` (and `pypy` if available) |

**Measuring.**

- Time only the computation, not process start or printing.
- Run each case 5 times; report best and median.
- Record CPU model, RAM, OS, compiler version and flags.
- Do the $10^9$ run once per language first. Don't start $10^{10}$ until the $10^9$ time and memory are known.

**Variants worth comparing in one language (C is enough):**

- bitset vs byte array vs hash set
- do the memory-access patterns matter (arcs jump far, so cache misses are likely the bottleneck)?

```bash
# add build + run commands here once written
```

## Results

_Not run yet._

| Language | N | best (s) | median (s) | peak RAM | notes |
| --- | --- | --- | --- | --- | --- |
| | $10^6$ | | | | |
| | $10^9$ | | | | |

## Findings

_None yet._

## Next

- Multi-core is hard here (each step depends on the last), but the visited set could be checked for how it spreads over memory.
- Try $10^{10}$ if RAM allows (about 7.5 GB as a bitset, estimate).
