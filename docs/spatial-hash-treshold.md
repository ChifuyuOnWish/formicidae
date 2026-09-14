# Research: Spatial Hash Grid vs Linear Scan for Deposit Detection

Closes: Checkpoint 1 issue "Research: spatial hash grid vs linear scan for
deposit detection" (#3)

---

## 1. Cost and complexity comparison

### Linear scan (current implementation)

Every sensing query checks every live deposit and tests its distance. For a
single query this is O(D), where D is the number of live deposits at that
moment. Across a full tick, every agent runs one query, so total cost is
O(A x D), where A is the number of agents.

This is the naive approach already in `PheromoneManager.cpp`
(`QueryNearby`, `TryMergeIntoExisting`). Its main appeal is simplicity and
correctness: there is nothing to get wrong structurally, and it is easy to
reason about and debug while the merge/decay/origin rules themselves are
still being validated.

Its cost scales with total deposit count, not with how spread out those
deposits are. A colony with 50 deposits clustered near the nest costs the
same per query as 50 deposits spread across the whole map, since every
query still checks all 50 regardless of relevance.

### Spatial hash grid

The world is divided into fixed-size cells (a good starting cell size is
roughly the sensing radius). Each deposit is bucketed into the cell(s) it
overlaps. A query only checks deposits in the querying agent's own cell and
its immediate neighbors, rather than every live deposit.

Query cost becomes roughly O(k), where k is the average number of deposits
per cell near the query point, largely independent of total deposit count
D. Total per-tick cost becomes roughly O(A x k) instead of O(A x D).

The tradeoff is implementation and maintenance complexity: deposits need to
be rebucketed when they move (position drift on merge) or are removed
(decay to zero), and cell size becomes a tuning parameter in its own right,
too small and a query touches many cells, too large and each cell holds too
many deposits, degrading back toward linear-scan behavior within a cell.

---

## 2. Estimated threshold

An exact crossover point cannot be stated with confidence before real
profiling exists, this is explicitly the subject of the "Profile and
validate performance at target scale" issue in Checkpoint 4. What follows
is an order-of-magnitude estimate to inform the Checkpoint 1 to 4 plan, not
a final number.

**Rough budget reasoning:** targeting 60 fps leaves a 16.6ms frame budget.
Sensing is one system among many (rendering, physics, colony-view logic,
UI), so a conservative allocation might be 1 to 2ms for all agent sensing
combined. A single distance check (squared distance comparison, no sqrt) in
C++ is on the order of a few nanoseconds when data is laid out
cache-friendly in a flat array, say 10 to 30ns including loop overhead.

At A = 300 agents (mid-range of the "hundreds" target) and a 1.5ms budget:

```
1,500,000 ns / 300 agents ≈ 5,000 ns per agent
5,000 ns / ~20 ns per check ≈ 250 deposit checks per agent, per tick
```

That suggests linear scan stays affordable roughly up to D ≈ 250 live
deposits at A = 300 agents, before sensing alone risks eating the assumed
budget. Given the merge and strength-cap system already caps how many
deposits can exist in a given area (Section 3 of the deposit design doc),
D may well stay under this for a while even as agent count grows, since
busy areas collapse into fewer, stronger deposits rather than accumulating
one per ant.

This is a rough estimate only. Real numbers depend on actual merge
distance, sensing radius, and how spread out agents are across the map,
all of which are implementation and tuning decisions not yet finalized.
The number above should be treated as "a reason not to worry yet," not as
a committed threshold.

---

## 3. Decision

Keep the linear scan for Checkpoints 2 and 3 (core deposit system, single
agent). Correctness and ease of debugging matter more than performance
while the fundamental rules (merge, decay, origin resolution) are still
being validated, and a spatial structure adds complexity that would make
bugs in those rules harder to isolate.

Introduce the spatial hash grid at Checkpoint 4 (Swarm at Scale), where it
is already scoped as a conditional item: "Implement spatial hash grid (if
linear queries prove insufficient)." That checkpoint's own profiling issue
is the right place to gather real numbers and confirm or replace the
estimate in Section 2, rather than committing to the grid now based on a
rough calculation.

This defers the actual engineering cost of the grid (rebucketing on
move/removal, cell-size tuning) until there is real data justifying it,
while keeping the option identified and ready to implement rather than
discovered as a surprise once agent counts scale up.