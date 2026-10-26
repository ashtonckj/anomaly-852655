#!/usr/bin/env python3
"""Draw the Recamán sequence (OEIS A005132) as an arc diagram.

    python recaman.py                    # 10,000 terms -> output/recaman-10000.png
    python recaman.py -n 5000 -o my.png  # custom size / file
"""

import argparse
from pathlib import Path

import matplotlib

matplotlib.use("Agg")  # save to file only, no window needed
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.collections import LineCollection
from matplotlib.colors import LinearSegmentedColormap
from matplotlib.lines import Line2D

BG, GRID, TEXT, MUTED = "#0a1128", "#2a3a66", "#e8edf7", "#8fa0c4"
ORANGE = ["#ff5e00", "#ffc66e"]  # forward jumps  (+n), dark -> light
BLUE = ["#1f6bff", "#6fe3ff"]    # backward jumps (-n), dark -> light


def recaman(n_terms):
    """Return the first n_terms terms a(0) .. a(n_terms-1)."""
    seen = {0}
    seq = [0]
    for n in range(1, n_terms):
        back = seq[-1] - n
        nxt = back if back > 0 and back not in seen else seq[-1] + n
        seen.add(nxt)
        seq.append(nxt)
    return seq


def draw(seq, path, dpi=200):
    a = np.array(seq, dtype=float)
    start, end = a[:-1], a[1:]
    steps = len(start)
    centre, radius = (start + end) / 2, np.abs(end - start) / 2
    forward = end > start
    side = np.where(np.arange(1, steps + 1) % 2 == 1, 1.0, -1.0)  # alternate up / down

    # every step is a semicircle, drawn as a 64-point curve
    theta = np.linspace(0, np.pi, 64)
    direction = np.where(forward, -1.0, 1.0)[:, None]
    x = centre[:, None] + direction * radius[:, None] * np.cos(theta)
    y = side[:, None] * radius[:, None] * np.sin(theta)
    arcs = np.stack([x, y], axis=2)

    # colour: orange/blue by direction, dark -> light as n grows
    t = np.linspace(0, 1, steps)
    orange = LinearSegmentedColormap.from_list("o", ORANGE)(t)
    blue = LinearSegmentedColormap.from_list("b", BLUE)(t)
    colours = np.where(forward[:, None], orange, blue)
    colours[:, 3] = 0.85
    linewidth = float(np.clip(0.9 * (500 / steps) ** 0.5, 0.2, 1.4))  # thinner when dense

    # figure height follows the data so semicircles stay round
    max_val, half = a.max(), np.abs(y).max()
    pad = max_val * 0.015
    width_in = 16.0
    plot_h = np.clip(14.7 * (2.16 * half) / (max_val + 2 * pad), 2.5, 8.0)
    height_in = plot_h + 2.6
    fig = plt.figure(figsize=(width_in, height_in), dpi=dpi, facecolor=BG)
    ax = fig.add_axes([0.04, 1.1 / height_in, 0.92, plot_h / height_in], facecolor=BG)

    ax.add_collection(LineCollection(arcs, colors=colours, linewidths=linewidth, capstyle="round"))
    ax.set_xlim(-pad, max_val + pad)
    ax.set_ylim(-half * 1.08, half * 1.08)
    ax.set_aspect("equal", anchor="C")
    ax.axhline(0, color=GRID, linewidth=1.2, zorder=0)
    ax.grid(axis="x", color=GRID, alpha=0.5, linewidth=0.6)
    ax.set_axisbelow(True)
    ax.set_yticks([])
    ax.tick_params(axis="x", colors=MUTED, labelsize=10, length=0, pad=10)
    for spine in ax.spines.values():
        spine.set_visible(False)

    # text: title, subtitle, legend (no formula)
    fig.text(0.05, 1 - 0.6 / height_in, "The Recamán Sequence", color=TEXT,
             fontsize=34, fontweight="bold", va="center")
    fig.text(0.05, 1 - 1.15 / height_in,
             f"{len(seq):,} terms  ·  largest value {int(max_val):,}",
             color=MUTED, fontsize=14, va="center")
    handles = [Line2D([0], [0], color=ORANGE[1], lw=3, label="forward jump (+n)"),
               Line2D([0], [0], color=BLUE[1], lw=3, label="backward jump (−n)")]
    legend = fig.legend(handles=handles, loc="center right", ncol=2, frameon=False,
                        bbox_to_anchor=(0.96, 0.4 / height_in), fontsize=11.5)
    for text in legend.get_texts():
        text.set_color(MUTED)

    path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(path, dpi=dpi, facecolor=BG)
    plt.close(fig)


def main():
    p = argparse.ArgumentParser(description="Draw the Recamán sequence as an arc diagram.")
    p.add_argument("-n", "--terms", type=int, default=10_000, help="number of terms (default 10000)")
    p.add_argument("-o", "--output", type=Path, help="output PNG (default output/recaman-<n>.png)")
    p.add_argument("--dpi", type=int, default=200, help="resolution (default 200 = 3200 px wide)")
    args = p.parse_args()
    if args.terms < 2:
        p.error("--terms must be at least 2")

    out = args.output or Path("output") / f"recaman-{args.terms}.png"
    draw(recaman(args.terms), out, args.dpi)
    print(f"Saved {out}")


if __name__ == "__main__":
    main()
