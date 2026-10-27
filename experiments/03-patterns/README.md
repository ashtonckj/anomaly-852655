# 03: Rings, ceilings and predictions

[← All experiments](../README.md)

**Status:** done (first pass), predictions open

## Question

The big "holes" in the arc diagram are rings made by long zigzag runs. Are their sizes and colours predictable?

## Method

`scan.c` runs the sequence to $10^9$ terms (3 s) and logs every long zigzag run (a stretch where steps strictly alternate back/forward). `analyze.py` groups runs by their ceiling (the highest value they reach), calls a group with 40+ runs a **ring**, then fits and tests the ceilings.

```bash
gcc -O3 -march=native -o scan scan.c
./scan 1000000000 runs.csv checkpoints.csv     # about 3 s, about 1.1 GB RAM
python analyze.py
```

`runs.csv` and `checkpoints.csv` from the original run are included, so `python analyze.py` works without compiling.

## What a ring is

Inside a zigzag run the sequence alternates $x \to x-n \to x+1 \to x-n-1 \to \dots$: two tracks, one climbing by 1 and one falling by 1 every two steps. So the run sweeps the range from `hi − n` to `hi`, and its arcs are all nearly the same size. Stacked together they form a thick ring with a hollow middle. Measured over all long runs, (hi − lo) / n at the end of the run has median 1.000.

## Results

12 rings found between $2\times10^6$ and $10^9$:

| Ring | Ceiling | Ratio to previous | Forward-step parity |
| --- | --- | --- | --- |
| 0 | 2,046,272 | | even |
| 1 | 3,480,177 | 1.70 | odd |
| 2 | 6,336,136 | 1.82 | even |
| 3 | 11,496,649 | 1.81 | odd |
| 4 | 20,384,018 | 1.77 | even |
| 5 | 36,382,380 | 1.79 | odd |
| 6 | 63,829,466 | 1.75 | even |
| 7 | 115,222,415 | 1.81 | odd |
| 8 | 202,002,246 | 1.75 | even |
| 9 | 344,699,964 | 1.71 | odd |
| 10 | 602,660,174 | 1.75 | even |
| 11 | 1,018,152,796 | 1.69 | odd |

## Findings

1. **Ceilings are roughly geometric.** Each ring's ceiling is about 1.77 times the previous one (mean 1.76, sd 0.05).
2. **Hold-out test.** Fitting only the first 5 rings and predicting the other 7 gives a mean error of 5%, but the error grows with distance (0.3% for ring 5, 15% for ring 11). The ratio seems to drift slowly down (about 1.8 early, about 1.7 late), so a constant ratio slightly overshoots. Not yet clear whether it converges or keeps drifting.
3. **Colour alternates every ring.** Forward jumps in a ring all have the same parity of $n$, because zigzag steps alternate. Odd parity puts orange arcs on top, even puts them below. Across all 12 rings the parity alternates perfectly (`010101010101`, purity 92% to 100% per ring). So the colour of a ring is predictable from the previous one.
4. **About 4 new rings per ×10 in $N$.** Rings are spaced by ×1.77 and the maximum grows roughly in line with $N$, so a decade of terms adds $\ln 10 / \ln 1.77 \approx 4$ rings. Observed: 12 rings over about 3 decades.
5. **1355 is still missing at $10^9$ terms.** The smallest unvisited number went 4, 19, 61, 879, 1355 and has not moved since $N \approx 5\times10^5$.
6. **Most of the number line stays empty.** At every checkpoint from $2\times10^5$ to $10^9$, 87% to 90% of the integers below the maximum are unvisited. The maximum is about $6$ to $7\,N$ at large $N$ (3.7× at $10^3$).

## Predictions (open)

Check these by running further. $10^9$ is the limit of this scanner on a 3 GB machine; `./scan 3000000000 ...` needs about 2 to 4 GB.

| # | Prediction | Basis | Status |
| --- | --- | --- | --- |
| P1 | Ring 12 has ceiling between 1.75 and 1.9 billion and **even** parity (orange arcs below, blue above) | ratio 1.7 to 1.77 from ring 11, alternation | partly seen: a ceiling near 1.80 billion with 14 runs already exists at $10^9$ terms, but is not heavy yet |
| P2 | Ring 13 has ceiling between 3.1 and 3.4 billion and **odd** parity (orange on top) | same | open |
| P3 | A picture at $N = 10^{10}$ shows about 4 more rings than one at $N = 10^9$ | finding 4 | open |
| P4 | 1355 stays missing past $10^9$ terms | finding 5 | confirmed up to $10^9$, no prediction beyond |

## Caveats

- Ring detection used choices made after looking at the data (group within 2%, at least 40 runs). The cut is not arbitrary: the 12 rings have 48 to 104 runs, and every other group has 22 or fewer.
- This is a fit to observed data, not a proof. Nothing here explains *why* the ratio is near 1.77, and it matches no constant I know of.
- Only one start value (0) and one run.

## Next

- Verify P1/P2 by running past $10^9$ terms.
- Test whether the ratio converges (log-log fit over more rings).
- Does a different start value (experiment 01) change the ratio?
