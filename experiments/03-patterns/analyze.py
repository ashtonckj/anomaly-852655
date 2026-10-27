"""Analyse runs.csv / checkpoints.csv written by scan.c  (experiment 03).

    python analyze.py            # reads runs.csv and checkpoints.csv in this folder

Method (kept simple on purpose):
  1. Every long zigzag run has a ceiling `hi`. Group runs whose ceilings are within 2% of each other.
  2. A group with >= MIN_RUNS runs is a "ring" (a big circle in the picture); its ceiling is the median hi.
  3. Fit  log(ceiling) = a + k * log(r)  over the rings, and test the fit on rings it has not seen.
"""
import numpy as np
import pandas as pd

MIN_RUNS = 40   # a ring needs at least this many long runs (natural gap: rings have 48+, the rest have <= 22)
TRAIN = 5       # fit on the first TRAIN rings only, predict the rest

runs = pd.read_csv("runs.csv")
runs["n_end"] = runs.n_start + runs.length
runs = runs.sort_values("hi").reset_index(drop=True)
runs["group"] = (runs.hi > runs.hi.shift() * 1.02).cumsum()

rings = (runs.groupby("group")
         .agg(ceiling=("hi", "median"), runs=("hi", "size"), n_first=("n_start", "min"),
              n_last=("n_end", "max"), parity=("fwd_parity", lambda s: int(s.mean() >= 0.5)),
              purity=("fwd_parity", lambda s: max(s.mean(), 1 - s.mean())))
         .query("runs >= @MIN_RUNS").sort_values("ceiling").reset_index(drop=True))
rings["ratio"] = rings.ceiling / rings.ceiling.shift()

print("zigzag width / n_end (median):", round(((runs.hi - runs.lo) / runs.n_end).median(), 3))
print("\nRINGS (heavy ceilings)")
print(rings.round(3).to_string())

k = np.arange(len(rings))
slope, icpt = np.polyfit(k, np.log(rings.ceiling), 1)
print(f"\nall rings: ratio per ring r = {np.exp(slope):.3f}   (mean of ratios {rings.ratio.mean():.3f}, sd {rings.ratio.std():.3f})")

# hold-out test: fit on the first TRAIN rings, predict the others
s2, i2 = np.polyfit(k[:TRAIN], np.log(rings.ceiling[:TRAIN]), 1)
pred = np.exp(i2 + s2 * k)
test = pd.DataFrame({"ceiling": rings.ceiling, "predicted": pred, "error_%": (pred / rings.ceiling - 1) * 100})[TRAIN:]
print(f"\nHOLD-OUT: fit on first {TRAIN} rings (r = {np.exp(s2):.3f}), predict the remaining {len(rings) - TRAIN}")
print(test.round(1).to_string())
print(f"mean |error| = {test['error_%'].abs().mean():.1f}%   worst = {test['error_%'].abs().max():.1f}%")

print("\nNEXT RINGS (fit on all rings)")
for j in range(len(rings), len(rings) + 4):
    print(f"  ring {j}: ceiling ~ {np.exp(icpt + slope * j):,.0f}")

print("\nORIENTATION (parity of n on forward steps) in ring order:", "".join(str(p) for p in rings.parity))
print("alternates every ring?", all(rings.parity[i] != rings.parity[i + 1] for i in range(len(rings) - 1)))

cp = pd.read_csv("checkpoints.csv")
cp["max/N"] = (cp["max"] / cp.N).round(2)
cp["unvisited_%"] = (cp.unvisited_below_max / cp["max"] * 100).round(1)
print("\nCHECKPOINTS")
print(cp[cp.N >= 1000].to_string(index=False))
