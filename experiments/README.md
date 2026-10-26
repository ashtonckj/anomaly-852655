# Experiments

One folder per experiment. Each has its own `README.md` written from [`_template/`](_template/README.md).
Back to the [main README](../README.md).

## Index

| # | Experiment | Question | Status |
| --- | --- | --- | --- |
| 01 | [Custom start value](01-custom-start/README.md) | What changes if $a(0) = k$ instead of $0$? | planned |
| 02 | [Speed benchmark](02-speed-benchmark/README.md) | How fast can 1M / 1B terms be computed, and in which language? | running |

Status values: `idea` → `planned` → `running` → `done`.

## Findings log

One line per discovery. Link to the experiment that produced it.

| Finding | Source |
| --- | --- |
| 42 is the first value to repeat: $a(20) = a(24) = 42$ | baseline (Python, `recaman.py`) |
| The number 19 does not appear until $n = 99{,}734$ | baseline (Python, `recaman.py`) |
| Largest value so far is about $4n$ to $6n$ (3.8× at 10k terms, 5.9× at 1M, 6.7× at 1B) | baseline (Python, `recaman.py`) |
| C computes $10^9$ terms in 1.7 s: $a(10^9) = 3{,}802{,}905{,}932$, max $6{,}699{,}287{,}376$ | [02](02-speed-benchmark/README.md) |
| $a(10^6) = 2{,}057{,}164$; largest value in the first $10^6 + 1$ terms is $5{,}946{,}126$ | baseline (Python, `recaman.py`) |

## Idea backlog

Add a line here whenever something comes to mind. Promote it to a folder when you start it.

- **Hunt for 852655**: track the smallest missing number as the number of terms grows.
- **First-appearance time**: for each value $v$, at which $n$ does it first appear? Which values are slowest?
- **Forward vs backward ratio**: what fraction of steps are backward, and does it settle?
- **Other step rules**: step $2n$, step $n^2$, step $n + c$, or a different "forbidden" rule.
- **Minimised variant**: [OEIS A125717](https://oeis.org/A125717), a related sequence to compare against.
- **Growth of the maximum**: fit $\max a(n)$ against $n$ on a log-log plot.
- **Gaps**: how many unvisited "holes" remain below the current maximum?
- **Sound / animation**: turn the jumps into notes or a video.

## How to add an experiment

1. Copy `_template/` to `NN-short-name/` (next number, lowercase, hyphens).
2. Fill in the question and method first, results later.
3. Keep code, data and images inside that folder. Don't edit `recaman.py` for experiments; copy what you need.
4. Add a row to the **Index** above and to the table in the [main README](../README.md).
5. When you learn something, add one line to the **Findings log**.
