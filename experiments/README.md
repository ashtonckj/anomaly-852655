# Experiments

One folder per experiment. Each has its own `README.md` written from [`_template/`](_template/README.md).
Back to the [main README](../README.md).

## Index

| # | Experiment | Question | Status |
| --- | --- | --- | --- |
| 01 | [Custom start value](01-custom-start/README.md) | What changes if $a(0) = k$ instead of $0$? | planned |
| 02 | [Speed benchmark](02-speed-benchmark/README.md) | How fast can 1M / 1B terms be computed, and in which language? | running |
| 03 | [Rings, ceilings and predictions](03-patterns/README.md) | Are the big rings in the picture predictable? | done, predictions open |

Status values: `idea` → `planned` → `running` → `done`.

## Findings log

One line per discovery. Link to the experiment that produced it.

| Finding | Source |
| --- | --- |
| 42 is the first value to repeat: $a(20) = a(24) = 42$ | baseline (Python, `recaman.py`) |
| The number 19 does not appear until $n = 99{,}734$ | baseline (Python, `recaman.py`) |
| Largest value so far is about $4n$ to $6n$ (3.8× at 10k terms, 5.9× at 1M, 6.7× at 1B) | baseline (Python, `recaman.py`) |
| Zigzag rings have ceilings growing about ×1.77 each (12 rings, $2\times10^6$ to $10^9$) | [03](03-patterns/README.md) |
| The colour (orange/blue side) alternates on every ring | [03](03-patterns/README.md) |
| 1355 is still unvisited at $10^9$ terms; about 88% of integers below the maximum are unvisited | [03](03-patterns/README.md) |
| C computes $10^9$ terms in 1.7 s: $a(10^9) = 3{,}802{,}905{,}932$, max $6{,}699{,}287{,}376$ | [02](02-speed-benchmark/README.md) |
| $a(10^6) = 2{,}057{,}164$; largest value in the first $10^6 + 1$ terms is $5{,}946{,}126$ | baseline (Python, `recaman.py`) |

## Idea backlog

Add a line here whenever something comes to mind. Promote it to a folder when you start it.

- **Hunt for 852655**: the smallest missing number so far went 19, 61, 879, 1355 (see 03). Does it keep climbing?
- **First-appearance time**: for each value $v$, at which $n$ does it first appear? Which values are slowest?
- **Forward vs backward ratio**: what fraction of steps are backward, and does it settle?
- **Other step rules**: step $2n$, step $n^2$, step $n + c$, or a different "forbidden" rule.
- **Minimised variant**: [OEIS A125717](https://oeis.org/A125717), a related sequence to compare against.
- **Does the ring ratio converge?** Ceiling ratio looks like 1.8 early, 1.7 late. Run past $10^9$ to see.
- **Gaps**: how many unvisited "holes" remain below the current maximum?
- **Sound / animation**: turn the jumps into notes or a video.

## How to add an experiment

1. Copy `_template/` to `NN-short-name/` (next number, lowercase, hyphens).
2. Fill in the question and method first, results later.
3. Keep code, data and images inside that folder. Don't edit `recaman.py` for experiments; copy what you need.
4. Add a row to the **Index** above and to the table in the [main README](../README.md).
5. When you learn something, add one line to the **Findings log**.
