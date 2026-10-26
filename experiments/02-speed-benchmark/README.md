# 02: Speed benchmark

[← All experiments](../README.md)

**Status:** running

## Question

How long does it take to compute $10^6$, $10^9$ (and maybe $10^{10}$) terms of the sequence, with no plotting, and which language is fastest?

## Method

**Task (identical in every language):** compute $a(0) \dots a(N)$, keep only the last value and the largest value, print them. No arrays of terms are stored and no images are made.

**Correctness check.** Every implementation must print the same numbers. Known reference from the Python baseline:

| N | a(N) | largest value |
| --- | --- | --- |
| $10^6$ | 2,057,164 | 5,946,126 |
| $10^9$ | 3,802,905,932 | 6,699,287,376 |

**Visited set.** The hot loop is dominated by "have I seen this number?", so use a **bitset** (1 bit per integer) rather than a byte array or hash set. The largest value was about $6\times$ the number of terms in earlier runs, and at $10^9$ it was $6.7\times$, so $10^9$ terms needs about $6.7\times10^9$ bits (the C version rounds up to 1.07 GB). Make sure the machine has the RAM.

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

## How to run

C version (`recaman.c`, already written):

```bash
gcc -O3 -march=native -o recaman recaman.c
./recaman 1000000          # Windows: recaman.exe 1000000
```

On Windows, install a compiler once: `winget install BrechtSanders.WinLibs.POSIX.UCRT`, then reopen the terminal and check `gcc --version`.

For other languages, the same steps apply: install the toolchain, build in release mode, run with $N$ as the argument.

| Language | Install (Windows) | Build and run |
| --- | --- | --- |
| Rust | [rustup.rs](https://rustup.rs) | `cargo new recaman`, then `cargo run --release -- 1000000` |
| Zig | `winget install zig.zig` | `zig build-exe recaman.zig -O ReleaseFast`, then `recaman.exe 1000000` |
| C++ | same compiler as C | `g++ -O3 -march=native -o recaman recaman.cpp` |
| Go | `winget install GoLang.Go` | `go build -o recaman.exe`, then `recaman.exe 1000000` |

Always build in release/optimised mode, otherwise the timing is meaningless.

## Results

First run, C only, single run per size, 1 CPU core in a Linux container (not your PC, so expect different times). Both outputs match the Python baseline at $10^6$.

| Language | N | time (s) | bitset | notes |
| --- | --- | --- | --- | --- |
| C (`gcc -O3 -march=native`) | $10^6$ | 0.002 | 1 MB | matches Python |
| C | $10^8$ | 0.212 | 134 MB | |
| C | $10^9$ | 1.690 | 1.07 GB | |

Still to do: repeat 5 times, record CPU model, add the other languages.

## Findings

- C does $10^9$ terms in under 2 s in this first run, about 1.7 ns per step. The access pattern is mostly local (steps stay near recent values), so cache misses hurt less than expected.
- Time grows roughly linearly with $N$ ($10^8$ to $10^9$ is about 8x).

## Next

- Multi-core is hard here (each step depends on the last), but the visited set could be checked for how it spreads over memory.
- Try $10^{10}$ if RAM allows (roughly 8.6 GB to 17 GB as a bitset, estimate).
