# Architecture Justification: FPheromoneDeposit / UPheromoneManager

Closes: Checkpoint 1 issue "Architecture justification:
FPheromoneDeposit / UPheromoneManager"

---

## 1. Subsystem responsibilities

### FPheromoneDeposit: data only, no behavior

Holds the state of a single deposit: position, strength, decay rate, max
strength, origin (`EPheromoneOrigin`), type (`EPheromoneType`), and the
redirection-specific lifetime clock. Its only logic is a pair of small
accessors (`GetRadius()`, `IsExpired()`) derived directly from its own
fields. It does not query other deposits, does not know how to merge with
another deposit, and does not manage its own lifetime beyond exposing
whether it has expired.

This is deliberate. A deposit is meant to be dumb data that many deposits
can exist as, cheaply, not an actor in its own right.

### UPheromoneManager: owns all deposits, all rules, and all queries

Everything a deposit cannot do for itself, the manager does on its
behalf, for every deposit at once:

- **Storage.** Owns the single collection of live deposits.
- **Decay.** Runs every tick, applies to every deposit unconditionally,
  including deposits at the strength cap (see the deposit design doc,
  the cap limits ceiling, it does not grant durability).
- **Merging.** Neighbor lookup on creation, weighted position drift,
  strength combination up to the cap, and origin OR-resolution
  (`Ant` beats `Player` on merge, never the reverse).
- **Querying.** The single interface (`QueryNearby`) that ant sensing and
  debug visualization both read through.
- **Redirection validity.** Anchor checking (`IsValidRedirectionAnchor`)
  and placement (`TryPlaceRedirection`), enforcing the double-anchor rule
  and the independent, non-self-reinforcing decay clock.

Concretely: **it is the single source of truth for pheromone state and
the only place the merge/decay/origin rules are implemented.** Ant
agents and the player's drawing tools both call into this one interface
rather than each implementing their own version of these rules, which is
what keeps the two capable of producing identical-looking pheromone
behavior despite having different callers.

### What the manager deliberately does not own

Per-agent scheduling (when a given ant next re-evaluates its heading, the
throttled decision step from the update-frequency research) is not the
manager's responsibility. Each agent owns its own wake state and simply
calls into the manager's query interface whenever it decides to
re-evaluate. This keeps the manager stateless with respect to agents,
it has no per-agent bookkeeping to maintain, and keeps the throttling
logic (which is a per-agent concern) from leaking into a subsystem meant
to describe the pheromone field itself.

---

## 2. Struct vs UObject, justified

### FPheromoneDeposit is a USTRUCT, not a UObject

Three reasons, in order of how much they actually drove the decision:

1. **Volume.** Design targets hundreds of concurrent agents, each
   depositing repeatedly. Even with merging and the strength cap keeping
   the live count bounded, the number of deposits alive at once is
   expected to be far higher than the number of agents. Each `UObject`
   carries real per-instance overhead: a distinct heap allocation, garbage
   collector tracking, and reflection bookkeeping. None of that is needed
   for a value type that is just a handful of floats and two small enums.
2. **Storage and access pattern.** Structs stored contiguously in a
   `TArray` inside the manager are cache-friendly to iterate, which
   matters directly for the linear-scan cost identified in the spatial
   hash grid research (`O(A x D)`), every deposit gets touched every tick
   during decay, and every query touches a meaningful fraction of them
   until the grid is introduced. A `UObject`-based deposit would add
   pointer-chasing overhead to a loop that is already the primary
   performance concern at scale.
3. **Lifetime is trivial.** A deposit's lifetime is fully owned by the
   manager's array, added by `Deposit()`, removed when `IsExpired()`
   becomes true during the decay pass. There is no case where a deposit
   needs to be referenced from multiple places, persist independently of
   the manager, or participate in Blueprint-side object graphs on its own,
   the properties that would justify `UObject`'s overhead in exchange for
   its features.

### UPheromoneManager is a UObject (specifically, a UWorldSubsystem)

The opposite reasoning applies here, there is exactly one manager per
world, and it benefits directly from what `UObject` provides:

1. **Lifecycle integration.** As a `UWorldSubsystem`, it is created and
   destroyed automatically with the world. No manual singleton setup or
   teardown code is needed, and there is no risk of it outliving or
   being created before the world it belongs to.
2. **Global, typed access.** Reachable from anywhere via
   `GetWorld()->GetSubsystem<UPheromoneManager>()`, rather than requiring
   a manually threaded reference passed down to every ant agent and every
   player tool that needs to call into it.
3. **Editor-exposed tuning.** Tuning values (`MergeDistance`,
   `MinAnchorStrength`) are `UPROPERTY(EditAnywhere)`, editable directly
   in the editor without recompiling, which matters for iterating on feel
   during Checkpoint 4 calibration. This is a `UObject`-only capability.
4. **One instance, not many.** Unlike deposits, there is no volume
   argument against `UObject` overhead here, one manager existing per
   world makes the per-instance cost irrelevant.

### Why the split matters architecturally, not just per-type

Because the manager is the only thing that queries, merges, or decays
deposits, swapping the internal implementation of `QueryNearby` from a
linear scan to a spatial hash grid (Checkpoint 4, if profiling justifies
it per the spatial hash grid research) is entirely internal to the
manager. Ant agents and player tools call the same interface either way,
and `FPheromoneDeposit` itself does not change. The struct/UObject split
is what makes that boundary clean, the manager is the one place with
`UObject` lifecycle and reflection, everything below it is plain data
with no such coupling to worry about during a later internal rewrite.

---

## 3. Presentation and validation

This document is intended to be walked through directly at the
Checkpoint 1 defense, structured so that Section 1 and 2 above can be
read in order as the presentation itself: what each part owns, then why
each part is the type it is. Validation of this acceptance item happens
at that defense rather than through additional written content here.