# Research: Update Frequency / Potential Parallelization

Closes: Checkpoint 1 issue "Research: update frequency / potential
parallelization" (#4)

---

## 1. Impact of reduced tick frequency on emergent behavior

### What should stay at full rate, and what shouldn't

Two things happen per agent per tick: moving along the current heading and
depositing pheromone, and deciding whether that heading should change. These
do not need the same frequency.

Movement and deposit should stay at full tick rate. Reducing either would
directly degrade trail resolution and make motion visibly choppy, exactly
the two things the emergent-trail mechanic depends on looking convincing.

The decision step, sensing nearby deposits and recomputing a heading, is
the part that can run less often without necessarily costing anything
behaviorally. This matches observed real ant movement, which alternates
short straight runs with reorientation events rather than continuously
curving toward a target every instant. A throttled decision step is not
purely a performance shortcut here, it plausibly produces more convincing
movement than re-deciding every tick would.

### Where throttling is safe, and where it isn't

An agent on a strong, established trail has a stable heading, the local
pheromone landscape around it is not changing quickly tick to tick, so
infrequent re-evaluation costs little. An agent exploring unclaimed ground
is in the opposite situation, the local landscape can change as soon as it
or a nearby agent lays new pheromone, so infrequent re-evaluation there
risks agents missing a newly-formed trail and wandering past it.

This suggests the interval should not be a single global constant. A
context-scaled interval, short for exploring agents, longer for agents on
a strong trail, keeps the areas where behavior actually needs
responsiveness cheap to evaluate often, while spending less budget where
nothing is likely to have changed.

### Hard cases that must bypass any timer

Two mechanics already specified elsewhere in the design depend on
near-immediate response and cannot tolerate being delayed by a stale
interval:

- **Alarm pheromone entering sensing range.** A delayed reaction to danger
  defeats the purpose of the alarm mechanic.
- **Redirection at a junction.** Redirection is defined as overriding the
  normal routing decision at a specific point. An agent that doesn't
  re-evaluate promptly on reaching that point would simply walk past the
  redirection as if it weren't there.

Both should force an immediate wake regardless of any periodic interval,
rather than being subject to it. A small set of hard wake triggers,
alongside a periodic baseline, is preferable to relying on the periodic
interval alone.

### Assessed impact

With movement and deposit unaffected, and forced wake triggers covering
the cases where delay would break a specific mechanic, throttling the
decision step is assessed as low risk to emergent behavior, and plausibly
an improvement to how natural movement looks, provided the interval is
context-scaled rather than a single fixed value applied uniformly
regardless of agent situation.

---

## 2. Parallelization approaches identified

Not selected or implemented at this stage, listed here to inform later
decisions once real profiling data exists (see the related "Research:
spatial hash grid vs linear scan" and "Profile and validate performance at
target scale" issues).

### Phase-split parallelism (native Unreal tooling)

Splitting a tick into a read-only decision phase and a write phase is the
most direct fit for Unreal's existing `ParallelFor` / Task Graph system.
The sensing query itself is embarrassingly parallel, each agent's decision
depends only on nearby deposit state, not on any other agent's decision
made in the same tick, so many agents' decisions can be computed
concurrently with no coordination needed between them.

The complication is the write phase. Depositing pheromone and merging
deposits touches shared state (the deposit structure, or its spatial
index once one exists), so naive concurrent writes risk race conditions.
Two options worth investigating once this becomes relevant:

- **Defer and batch writes.** Each agent's tick produces a small list of
  intended deposits during the parallel decision phase, applied serially
  in a single pass afterward. Simple, correct, but the write pass itself
  stays single-threaded.
- **Spatial partitioning of the write phase.** If deposits are bucketed
  into cells (see the spatial hash grid research), agents writing into
  different cells don't conflict, so the write phase could itself be
  parallelized by partitioning work along cell boundaries rather than by
  agent. More complex to implement correctly, deferred until there is a
  concrete performance reason to need it.

### GPU compute (noted, not currently justified)

Massively parallel agent simulation via compute shaders exists as a known
technique in crowd-simulation contexts, and would be the natural next
step if agent counts ever grew far beyond the current hundreds-of-agents
target. Not pursued now, the current scale does not justify the
implementation cost and added complexity, and Unreal's CPU-side
`ParallelFor` is a much smaller step from the current single-threaded
implementation.

### Relationship to reduced tick frequency

Throttling (Section 1) and parallelization (this section) are
complementary rather than competing approaches. Throttling reduces the
total amount of decision work that needs to happen at all; parallelization
spreads whatever work remains across available cores. Both can be applied
together, and neither depends on the other being implemented first.

---

## 3. Where this fits the project plan

Both findings are inputs to Checkpoint 4 (Swarm at Scale), specifically
the "Calibrate merge/decay at scale" and "Profile and validate
performance at target scale" issues, rather than something to implement
during Checkpoint 1 or 2. Recording the approach now means Checkpoint 4
starts from an informed starting point (context-scaled decision interval
plus hard wake triggers, phase-split parallelism as the first
parallelization candidate) instead of researching from a blank slate once
performance work actually begins.