# Research: Pheromone-Following Algorithm at Large Scale

Closes: Checkpoint 1 issue "Research: pheromone-following algorithm at large
scale"

---

## 1. Approaches examined

### 1.1 Ant Colony Optimization (ACO)

**Sources:**
- Dorigo, M., Maniezzo, V., Colorni, A. (1996). *Ant System: Optimization by
  a Colony of Cooperating Agents.* IEEE Transactions on Systems, Man, and
  Cybernetics.
- Dorigo, M., Gambardella, L.M. (1997). *Ant Colony System: A Cooperative
  Learning Approach to the Traveling Salesman Problem.* IEEE Transactions on
  Evolutionary Computation.

**Core mechanism:** at each decision point, an agent picks a direction with
probability proportional to pheromone strength (and optionally a heuristic
factor), reinforces the path it took, and all pheromone evaporates over time
regardless of use. Ant Colony System (the 1997 variant) updates pheromone
online, per-step, rather than in batched cycles. This is closer to our model
than
the original 1996 formulation.

**How it applies here:** this is not something to adopt from scratch. Our
existing deposit system (continuous-space strength, radius-weighted sensing,
decay, merge) already implements the same three core rules (probabilistic
choice, reinforcement, evaporation) that ACO formalizes. The graph structure
classic ACO is usually described on is not essential to the algorithm; it's
just how the choice set was represented for the discrete problems (TSP, etc.)
ACO was originally built for. Our dynamically-queried "deposits within
sensing radius" plays the same role as an ACO edge set, just built from
continuous space instead of pre-authored.

Practical use: ACO literature gives us reference points for tuning constants
we already need, specifically the evaporation-rate-vs-reinforcement-rate
ratio, and known failure modes to watch for while calibrating it (too slow
evaporation → the colony fixates on a mediocre trail and stops exploring;
too fast → trails never stabilize).

*Note on ACOR:* a continuous-domain ACO variant exists (Socha & Dorigo,
2008), but it solves continuous **function optimization** (searching a
parameter space), not continuous **physical navigation**. It was reviewed
and ruled out as not applicable to our case despite the name similarity.

### 1.2 Artificial Potential Fields (APF)

**Source:**
- Khatib, O. (1986). *Real-Time Obstacle Avoidance for Manipulators and
  Mobile Robots.* The International Journal of Robotics Research, 5(1),
  90–98. DOI: 10.1177/027836498600500106.

**Core mechanism:** every relevant object in the environment generates a
vector field: attractive fields pull an agent toward goals, and repulsive
fields push it away from obstacles. The agent's next move is the
summed vector of every nearby field at its current position. Originally a
robotics technique for physical obstacle avoidance, not a
signal-following/foraging algorithm.

**How it applies here:** APF is not a competing system assigned to a
different category of object than pheromones. It is the mechanism that
already governs pheromone sensing: a deposit pulling an agent toward it is
an attractive potential field, exactly as APF describes. There is no reason
to wall APF off from pheromones and reserve it for obstacles only.

That said, APF was reviewed and ultimately not adopted for static obstacles
specifically. A field has influence at a distance by definition, an agent
starts reacting to it before making contact. That is correct for a
pheromone, since sensing something before arriving is the whole point, but
it is wrong for a rock or tree an agent has never encountered before. Using
APF for obstacles would give agents advance knowledge of a hazard they have
no in-world way of knowing about yet. See Section 2 for the approach chosen
instead.

---

## 2. Chosen approach

**Sensing and movement decision (pheromones):** Artificial Potential
Fields, applied to every nearby pheromone source. Each tick, an agent sums
the weighted vectors from forage trail and player trail (attracting) and
alarm (repelling), plus a small random term for exploration, and moves
along the result.

**Source content and lifecycle:** ACO-aligned rules continue to govern
deposits specifically, decay, merge, reinforcement, and origin tracking.
Alarm pheromone is a deposit like any other (ant-laid, decaying, sensed
within a radius), not an ambient force, so that hazard avoidance stays
diegetic and has a real in-world cause.

**Static obstacles: reactive local deflection, not a field.** An agent
computes its desired position from pheromone sensing alone, with no
knowledge of nearby obstacles feeding into that calculation. Only at the
point of moving is a check made: is the intended position blocked? If so,
the agent deflects, sliding along the obstacle's surface or rotating its
desired vector until it finds a clear direction, for that tick only,
with nothing remembered afterward. This avoids granting agents advance
awareness of an obstacle they have never physically encountered, which a
field-based approach would do by construction. It also means no separate
long-term obstacle memory is needed: a direction that is physically
blocked never accumulates a reinforced trail through it, so pheromone
reinforcement naturally routes future agents elsewhere on its own.

**Why this split:** pheromones are information an agent can and should
sense before arrival, that is what makes them useful as trail markers, so
a field is the correct model. A rock is not information, it has no
existence as something to be sensed at range, an agent only discovers it
by being physically obstructed. Modeling it as a field would blur that
distinction and grant unearned anticipation. A reactive check preserves it,
and is cheaper besides, a boolean collision test at one position instead of
a distance-weighted sum over every nearby obstacle.

---

## 3. Performance risks at target agent count (hundreds of concurrent agents)

- **Collision check cost at scale.** Every agent now needs a
  blocked-position check against static geometry each tick, in addition to
  the existing pheromone-deposit query. This is much cheaper per check than
  the deposit query, a boolean test rather than a distance-weighted sum,
  but it still runs once per agent per tick, so it is worth watching at
  hundreds of concurrent agents rather than assumed free.
  - *Mitigation:* obstacles are static and never move, merge, or expire, so
    their spatial structure can be built once and reused every tick rather
    than rebuilt, unlike the deposit structure, which must account for
    constantly changing data. Unreal's own collision/overlap system likely
    already provides this efficiently, so a custom structure may not be
    needed at all, worth confirming during implementation before building
    anything bespoke.
- **Deflection stability.** Repeated small deflections near a cluster of
  obstacles could, in principle, produce jittery or oscillating movement if
  the deflection logic is too reactive to its own previous deflection. Not
  a scaling risk as such, but worth a specific test case once obstacles are
  in place, an agent approaching a corner or tight gap between two
  obstacles, to confirm it resolves cleanly.
- **Pheromone-side tuning is unaffected.** Since obstacles no longer
  contribute a vector into the same summed calculation as pheromones, the
  earlier concern about needing to jointly tune pheromone strength against
  obstacle repulsion strength no longer applies. Pheromone tuning and
  obstacle deflection can be tuned independently.

---

## 4. Conclusion

No new sensing architecture is required for pheromones. APF already
describes the summation our sensing function performs across forage trail,
player trail, and alarm. ACO's rules continue to govern deposits
specifically: decay, merge, reinforcement, and the requirement that alarm
must have a real ant-laid origin rather than existing as an ambient force.

Static obstacles are handled separately, and deliberately not as a field.
An agent has no way of knowing about a rock it has never encountered, so
granting it advance sensing range over physical geometry would be
inconsistent with how the rest of the system works, information only
exists because an ant put it there, and a rock puts nothing there. Instead,
obstacles are resolved with a reactive check at the moment of movement:
proceed if clear, deflect locally if blocked, remember nothing afterward.
Long-term avoidance of a genuinely bad route falls out naturally from the
pheromone system already in place, since a blocked direction never
accumulates a reinforced trail, without needing any dedicated obstacle
memory of its own.