#!/usr/bin/env bash
# Creates the starter issue set via GitHub CLI.
# Each issue is its own readable block below (title, labels, milestone,
# acceptance checklist) — edit/add/remove blocks directly, no delimiter
# parsing to worry about.
#
# Run AFTER labels.sh and milestones.sh (milestone titles must match
# milestones.sh exactly).
set -euo pipefail

REPO="${1:-}"  # optional: owner/repo. Defaults to current repo.
REPO_FLAG=()
if [[ -n "$REPO" ]]; then
  REPO_FLAG=(--repo "$REPO")
fi

# issue "<title>" "<labels,comma,sep>" "<milestone>" \
#   "acceptance item 1" \
#   "acceptance item 2" \
#   ...
issue() {
  local title="$1" labels="$2" milestone="$3"
  shift 3
  local body="## Acceptance requirements"$'\n'
  for item in "$@"; do
    body+="- [ ] ${item}"$'\n'
  done
  body+=$'\n'"_Auto-created from project roadmap. See CONTRIBUTING.md for branch/commit conventions._"

  gh issue create \
    --title "$title" \
    --body "$body" \
    --label "$labels" \
    --milestone "$milestone" \
    "${REPO_FLAG[@]}"

  echo "Created: $title"
}

# ============================================================
# Checkpoint 1 — Algorithm Research & Architecture
# ============================================================

issue "Research: pheromone-following algorithm at large scale" \
  "research,must-have,area/pheromone" \
  "Checkpoint 1 — Algorithm Research & Architecture" \
  "At least 2 existing approaches documented with sources" \
  "Summary note stating which approach was chosen and why" \
  "Performance risks identified for the targeted agent count"

issue "Research: spatial hash grid vs linear scan for deposit detection" \
  "research,must-have,area/pheromone" \
  "Checkpoint 1 — Algorithm Research & Architecture" \
  "Cost/complexity comparison of both approaches written up" \
  "Approximate threshold (agent count) where the grid becomes necessary estimated" \
  "Decision recorded in the architecture document"

issue "Research: update frequency / potential parallelization" \
  "research,should-have,area/pheromone" \
  "Checkpoint 1 — Algorithm Research & Architecture" \
  "Impact of a reduced tick frequency on emergent behavior evaluated" \
  "Parallelization approaches identified, even if not implemented yet"

issue "Architecture justification: FPheromoneDeposit / UPheromoneManager" \
  "research,must-have,area/pheromone" \
  "Checkpoint 1 — Algorithm Research & Architecture" \
  "Subsystem responsibilities described in writing" \
  "Struct vs UObject choice justified" \
  "Presented and validated at the Checkpoint 1 defense"

issue "Unreal/C++ environment setup (engine, VS Code, LFS)" \
  "chore,must-have" \
  "Checkpoint 1 — Algorithm Research & Architecture" \
  "Unreal Editor launches without error" \
  "IntelliSense working via compileCommands" \
  "Git LFS configured and tested with a binary file"

issue "Version control pipeline setup (branches, labels, milestones, CI mirror)" \
  "chore,must-have" \
  "Checkpoint 1 — Algorithm Research & Architecture" \
  "Labels created and visible in the repo" \
  "Milestones created with correct dates" \
  "Mirror workflow functional (first run green)"

# ============================================================
# Checkpoint 2 — Core Deposit System
# ============================================================

issue "Implement FPheromoneDeposit (struct, decay, strength cap)" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 2 — Core Deposit System" \
  "Compiles with no warnings" \
  "Decay verified by manual test (value decreases over time)" \
  "Strength correctly capped even under excessive reinforcement"

issue "Implement UPheromoneManager: deposit + decay" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 2 — Core Deposit System" \
  "A call to Deposit() correctly creates an entry" \
  "Tick() decays all active entries" \
  "Entries at zero strength are removed"

issue "Implement deposit merging on creation (neighbor lookup)" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 2 — Core Deposit System" \
  "Two nearby deposits (within merge distance) merge into one object" \
  "A deposit outside range stays distinct" \
  "Edge case (exactly at merge distance) tested"

issue "Implement weighted position drift on merge" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 2 — Core Deposit System" \
  "Merged deposit position shifts toward the new deposit, proportional to its strength" \
  "Case of a much stronger deposit validated (little drift)" \
  "Case of equal strengths validated (drift to midpoint)"

issue "Implement the origin flag (Ant/Player) + OR-resolution on merge" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 2 — Core Deposit System" \
  "Player merged with Ant becomes Ant" \
  "Ant merged with Player stays Ant" \
  "Two Player deposits merged stay Player"

issue "Implement the strength cap without pausing decay" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 2 — Core Deposit System" \
  "A capped deposit continues decaying normally" \
  "Reinforcement beyond the cap doesn't exceed max strength" \
  "Regression test covering this case"

issue "Debug visualization for deposits (position, strength, origin)" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 2 — Core Deposit System" \
  "Strength reflected visually (color or size)" \
  "Ant vs Player origin visually distinguishable" \
  "Debug toggle can be turned on/off"

issue "Test harness for the deposit system" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 2 — Core Deposit System" \
  "Scenarios covering deposit, decay, merge, cap" \
  "Runnable without launching the full editor, if possible" \
  "Reproducible results"

# ============================================================
# Checkpoint 3 — Single Ant Agent
# ============================================================

issue "Ant agent: sensing nearby deposits (radius query)" \
  "feature,must-have,area/exploration" \
  "Checkpoint 3 — Single Ant Agent" \
  "Query returns only deposits within radius" \
  "Tested with 0, 1, and multiple deposits in range" \
  "No crash when no deposits are in range"

issue "Ant agent: direction weighting by strength/distance" \
  "feature,must-have,area/exploration" \
  "Checkpoint 3 — Single Ant Agent" \
  "A stronger deposit influences direction more than a weaker one" \
  "A closer deposit influences direction more than a farther one at equal strength" \
  "Behavior observable in a simple conflicting-deposits test case"

issue "Ant agent: weighted random walk (exploration/direction)" \
  "feature,must-have,area/exploration" \
  "Checkpoint 3 — Single Ant Agent" \
  "An ant with no deposit in range moves randomly" \
  "An ant with a strong deposit in range is visibly drawn toward it" \
  "Randomness factor adjustable via UPROPERTY"

issue "Ant agent: depositing pheromone while moving" \
  "feature,must-have,area/exploration" \
  "Checkpoint 3 — Single Ant Agent" \
  "Each movement step generates a deposit" \
  "Deposit frequency/amount adjustable" \
  "No abnormal buildup at a single fixed point (merge works correctly)"

issue "Test: a single agent forms a path to a static resource" \
  "feature,must-have,area/exploration" \
  "Checkpoint 3 — Single Ant Agent" \
  "Convergence toward the resource observed across multiple runs" \
  "Visual trace of the path observable at the end of a run" \
  "Behavior reproducible run to run"

# ============================================================
# Checkpoint 4 — Swarm at Scale
# ============================================================

issue "Spawn and manage a multi-agent swarm" \
  "feature,must-have,area/exploration" \
  "Checkpoint 4 — Swarm at Scale" \
  "Agent count configurable at launch" \
  "All agents tick without error" \
  "Clean agent teardown at end of run"

issue "Calibrate merge/decay at scale (hundreds of agents)" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 4 — Swarm at Scale" \
  "Paths visually coherent at target scale" \
  "Tuning parameters exposed via UPROPERTY" \
  "Chosen values documented"

issue "Profile and validate performance at target scale" \
  "research,must-have,area/pheromone" \
  "Checkpoint 4 — Swarm at Scale" \
  "Framerate measured and documented at target scale" \
  "Bottleneck identified if performance is insufficient" \
  "Profiling report kept (Unreal Insights capture or equivalent)"

issue "Implement spatial hash grid (if linear queries prove insufficient)" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 4 — Swarm at Scale" \
  "Neighbor queries use the grid instead of a full linear scan" \
  "Results identical to the previous implementation (non-regression)" \
  "Performance gain measured and documented"

issue "Test: visible path convergence across repeated runs" \
  "feature,must-have,area/exploration" \
  "Checkpoint 4 — Swarm at Scale" \
  "Multiple consecutive runs converge to a stable path" \
  "Behavior captured for the defense review" \
  "No abnormal divergence observed"

# ============================================================
# Checkpoint 5 — Player Pheromone Tools
# ============================================================

issue "Player tool: forage-trail drawing" \
  "feature,must-have,area/exploration" \
  "Checkpoint 5 — Player Pheromone Tools" \
  "Player can deposit pheromone at the designated location" \
  "The resulting deposit is correctly flagged Player" \
  "Behaves identically to an ant deposit for decay/merge"

issue "Player tool: redirection - double-anchor check" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 5 — Player Pheromone Tools" \
  "Placement refused if either endpoint isn't on a valid Ant deposit" \
  "Placement accepted if both endpoints are valid" \
  "Clear feedback on refusal"

issue "Player tool: redirection - independent, non-self-reinforcing decay" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 5 — Player Pheromone Tools" \
  "An ant walking the redirection doesn't extend its lifetime" \
  "Decay follows a fixed clock independent of traffic" \
  "Tested with and without traffic on the redirection"

issue "Player tool: redirection - expires if an anchor becomes invalid" \
  "feature,must-have,area/pheromone" \
  "Checkpoint 5 — Player Pheromone Tools" \
  "Redirection expires immediately if an anchor drops below the minimum threshold" \
  "No grace period observed" \
  "Tested on both anchors separately"

issue "Minimal tool-selection UI (forage/redirection)" \
  "feature,must-have,area/ui" \
  "Checkpoint 5 — Player Pheromone Tools" \
  "Player can switch between the two tools unambiguously" \
  "Active tool visible on screen" \
  "No accidental deposit when switching tools"

# ============================================================
# Checkpoint 6 — Scouting + First Hazard
# ============================================================

issue "Camera-based scouting / map reveal" \
  "feature,must-have,area/exploration" \
  "Checkpoint 6 — Scouting + First Hazard" \
  "Camera movement works without controlling a character" \
  "Unscouted areas visually distinct from scouted areas" \
  "Scouted state persists correctly over time"

issue "Restrict pheromone drawing to scouted areas" \
  "feature,must-have,area/exploration" \
  "Checkpoint 6 — Scouting + First Hazard" \
  "Drawing refused on an unscouted area" \
  "Drawing allowed once the area is scouted" \
  "Clear feedback on refusal"

issue "Rain/puddle hazard: trail washout" \
  "feature,must-have,area/hazard" \
  "Checkpoint 6 — Scouting + First Hazard" \
  "A rain event reduces or erases deposit strength in the affected area" \
  "Effect area clearly defined and tested" \
  "Effect reproducible via manual trigger for testing"

issue "Test: redirection used in response to a puddle" \
  "feature,must-have,area/hazard" \
  "Checkpoint 6 — Scouting + First Hazard" \
  "Full scenario: puddle washes out a trail, player places a redirection, ants take it" \
  "Redirection respects the double-anchor rule in this real scenario" \
  "New path observable after redirection"

# ============================================================
# Checkpoint 7 — Colony View Foundation
# ============================================================

issue "Colony view: 2D vertical cross-section (scene/camera)" \
  "feature,must-have,area/colony" \
  "Checkpoint 7 — Colony View Foundation" \
  "Switching to the colony view works" \
  "2D scene/camera renders correctly" \
  "Switching back to exploration view works without resetting state"

issue "Room: nursery" \
  "feature,must-have,area/colony" \
  "Checkpoint 7 — Colony View Foundation" \
  "Room is buildable from the UI" \
  "Room actually unlocks the associated caste" \
  "Room state persists across view switches"

issue "Room: fungus garden" \
  "feature,must-have,area/colony" \
  "Checkpoint 7 — Colony View Foundation" \
  "Room is buildable" \
  "Consumes resources brought back from the surface" \
  "Visible impact on colony growth"

issue "Room: storage/waste" \
  "feature,must-have,area/colony" \
  "Checkpoint 7 — Colony View Foundation" \
  "Room is buildable" \
  "Storage/waste function operational with a measurable effect" \
  "State persists across view switches"

issue "Resource flow: exploration to colony" \
  "feature,must-have,area/colony" \
  "Checkpoint 7 — Colony View Foundation" \
  "A resource brought back by an ant impacts colony state" \
  "Flow verifiable by observing both views in sequence" \
  "No silent resource loss (counts remain consistent)"

issue "Room construction cost (labor / time)" \
  "feature,must-have,area/colony" \
  "Checkpoint 7 — Colony View Foundation" \
  "Construction unavailable if labor is insufficient" \
  "Construction time observable (not instant)" \
  "Cost adjustable via UPROPERTY"

# ============================================================
# Checkpoint 8 — Caste System
# ============================================================

issue "Forager/nurse castes: target-ratio mechanic" \
  "feature,must-have,area/caste" \
  "Checkpoint 8 — Caste System" \
  "Target ratio adjustable by the player" \
  "New ratio doesn't instantly affect the existing population" \
  "Ratio value visible in the UI"

issue "Brood-cycle lag (delayed ratio application)" \
  "feature,must-have,area/caste" \
  "Checkpoint 8 — Caste System" \
  "Ratio changes take effect gradually, not instantly" \
  "Lag configurable and documented" \
  "Tested across a full ratio change (before/after)"

issue "Test: effect of caste ratio on foraging throughput" \
  "feature,must-have,area/caste" \
  "Checkpoint 8 — Caste System" \
  "Increasing the forager ratio measurably increases throughput" \
  "Decreasing it measurably reduces throughput" \
  "Result documented with before/after figures"

# ============================================================
# Checkpoint 9 — Full Loop Integration
# ============================================================

issue "Free switching between both views" \
  "feature,must-have,area/ui" \
  "Checkpoint 9 — Full Loop Integration" \
  "Switching possible at any time without blocking" \
  "Neither view pauses while the other is displayed" \
  "Tested across several consecutive switches"

issue "Independent update of both views (separate tick)" \
  "feature,must-have" \
  "Checkpoint 9 — Full Loop Integration" \
  "Colony view keeps evolving while the exploration view is displayed, and vice versa" \
  "Verified by comparing state before/after a switch" \
  "No state desync observed"

issue "Integration test: full end-to-end loop" \
  "feature,must-have" \
  "Checkpoint 9 — Full Loop Integration" \
  "Full run from a cold start to colony growth with no manual code intervention" \
  "Every step of the loop observed at least once" \
  "No crash over a reasonable run duration"

issue "Must-Have scope review (final checklist)" \
  "chore,must-have" \
  "Checkpoint 9 — Full Loop Integration" \
  "Every Must-Have item from the spec verified individually" \
  "Any gaps documented with justification" \
  "Review presented at the defense"

# ============================================================
# Checkpoint 10 — Should-Tier Scope
# ============================================================

issue "Phorid hazard: predation behavior" \
  "feature,should-have,area/hazard" \
  "Checkpoint 10 — Should-Tier Scope" \
  "Fly targets an unescorted ant on a trail" \
  "Attack frequency tied to trail traffic, as specified" \
  "Behavior observable and reproducible for demonstration"

issue "Minim-escort defense mechanic" \
  "feature,should-have,area/hazard" \
  "Checkpoint 10 — Should-Tier Scope" \
  "An escorted ant survives a phorid attack" \
  "An unescorted ant does not" \
  "Minim ratio affects escort coverage rate"

issue "Non-blocking issue-resolution system (meter)" \
  "feature,should-have,area/colony" \
  "Checkpoint 10 — Should-Tier Scope" \
  "Issue meter visible in the UI" \
  "Meter rises if ignored, falls if addressed" \
  "Resolution never blocks the main game loop"

issue "Escalating consequences for unresolved issues" \
  "feature,should-have,area/colony" \
  "Checkpoint 10 — Should-Tier Scope" \
  "At least three consequence tiers tested" \
  "Major consequence has a measurable impact on the colony" \
  "Thresholds used documented"

issue "Procedural resource placement (noise-based)" \
  "feature,should-have,area/procedural" \
  "Checkpoint 10 — Should-Tier Scope" \
  "Resources placed differently each run (variable seed)" \
  "Distribution stays playable/balanced across several tested seeds" \
  "Seed can be fixed manually to reproduce a given run"

issue "UI/UX pass: trail-strength visualization" \
  "feature,should-have,area/ui" \
  "Checkpoint 10 — Should-Tier Scope" \
  "Trail strength readable visually without debug mode" \
  "Clear differentiation between weak and strong trails" \
  "Readability feedback gathered if possible"

issue "UI/UX pass: room/caste management screen" \
  "feature,should-have,area/ui" \
  "Checkpoint 10 — Should-Tier Scope" \
  "Screen accessible from the colony view" \
  "Caste ratios adjustable from this screen" \
  "Room states visible at a glance"

issue "UI/UX pass: notification system" \
  "feature,should-have,area/ui" \
  "Checkpoint 10 — Should-Tier Scope" \
  "Notification appears for every event needing attention" \
  "Notification doesn't interrupt/block gameplay" \
  "Notifications reviewable after the fact"

# ============================================================
# Checkpoint 11 — Bonus / Polish
# ============================================================

issue "Second playable species (stretch)" \
  "feature,could-have" \
  "Checkpoint 11 — Bonus / Polish" \
  "Core mechanic genuinely different from the first species" \
  "Playable end-to-end for at least one full cycle" \
  "Documented to the same standard as the primary species"

issue "Deeper flora/fauna (stretch)" \
  "feature,could-have" \
  "Checkpoint 11 — Bonus / Polish" \
  "At least one new resource type with different fungus compatibility" \
  "At least one new hazard implemented" \
  "Gameplay impact observable and tested"

issue "Visual/audio polish pass" \
  "feature,could-have,area/ui" \
  "Checkpoint 11 — Bonus / Polish" \
  "Lighting/ambience improved with no readability regression" \
  "At least one key sound effect added" \
  "Informal visual review done before merge"

issue "Extended procedural systems (stretch)" \
  "feature,could-have,area/procedural" \
  "Checkpoint 11 — Bonus / Polish" \
  "At least one additional system implemented (seasonal rain cycle or terrain variation)" \
  "Behavior reproducible across several seeds" \
  "Documented to the same standard as base procedural placement"

# ============================================================
# Checkpoint 12 — Final Defense
# ============================================================

issue "Final project documentation" \
  "docs,must-have" \
  "Checkpoint 12 — Final Defense" \
  "Architecture documented for an external reader" \
  "Build/run instructions up to date" \
  "Scope decisions (what was actually delivered) documented"

issue "Final build / packaging" \
  "chore,must-have" \
  "Checkpoint 12 — Final Defense" \
  "Build launches without error on a clean machine, if possible" \
  "Build size and time documented" \
  "Packaging instructions reproducible"

issue "Final defense preparation" \
  "chore,must-have" \
  "Checkpoint 12 — Final Defense" \
  "Presentation materials ready" \
  "Demo rehearsed at least once under real conditions" \
  "Retrospective (delivered vs. planned) written"

echo "All 61 issues created."