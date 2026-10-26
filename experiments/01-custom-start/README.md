# 01: Custom start value

[← All experiments](../README.md)

**Status:** planned

## Question

Keep the Recamán rules but start at $a(0) = k$ for $k \neq 0$. Does the sequence still look and behave the same? Do the same numbers go missing?

## Method

Generalise the rule with a start value $k$:

$$a(0) = k, \qquad a(n) = \begin{cases} a(n-1) - n & \text{if } a(n-1) - n \geq 0 \text{ and unseen} \\ a(n-1) + n & \text{otherwise} \end{cases}$$

Decisions to make before running (write the choice down in Results):

- **Zero rule.** With $k = 0$, "positive" and "nonnegative" are the same, because 0 is already visited. With $k > 0$ they are not: may the sequence step onto 0? Run both versions.
- **Negative starts.** Should negative values be allowed at all? Try $k < 0$ only as a separate variant.

Runs to do:

- $k = 0$ as a control (must reproduce the baseline: `0, 1, 3, 6, 2, 7, …`)
- $k = 1, 2, 3, 5, 10, 100$
- 100,000 terms each, same stats for every run

Stats to record per run: largest value, number of distinct values, smallest missing number, first $n$ at which it repeats, backward-step fraction.

```bash
# add the script and exact command here once written
```

## Results

_Not run yet._

| k | largest value | distinct values | smallest missing | first repeat at n | backward fraction |
| --- | --- | --- | --- | --- | --- |
| 0 (control) | | | | | |

## Findings

_None yet._

## Next

- Compare arc diagrams for several $k$ side by side.
