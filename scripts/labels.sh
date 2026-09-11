#!/usr/bin/env bash
# Creates the label set for the Formicidae repo via GitHub CLI.
# Requires: gh CLI installed and authenticated (`gh auth login`), run from
# inside the repo (or set REPO below).
set -euo pipefail

REPO="${1:-}"  # optional: owner/repo, e.g. yourname/formicidae. Defaults to current repo.
REPO_FLAG=()
if [[ -n "$REPO" ]]; then
  REPO_FLAG=(--repo "$REPO")
fi

create_label() {
  local name="$1" color="$2" desc="$3"
  gh label create "$name" --color "$color" --description "$desc" "${REPO_FLAG[@]}" --force
}

# --- Priority (MoSCoW) ---
create_label "must-have"    "d73a4a" "Required for the project to succeed"
create_label "should-have"  "e99695" "Adds real value, attempted if on schedule"
create_label "could-have"   "0e8a16" "Stretch goal, only if ahead of schedule"

# --- Type ---
create_label "feature"   "1d76db" "New functionality"
create_label "bug"       "b60205" "Something broken"
create_label "research"  "5319e7" "Investigation/spike, not shippable as-is"
create_label "docs"      "0075ca" "Documentation only"
create_label "refactor"  "fbca04" "Code restructuring, no behavior change"
create_label "chore"     "c5def5" "Tooling, CI, repo config"

# --- Area ---
create_label "area/pheromone"    "f9d0c4" "Deposit/pheromone system"
create_label "area/exploration"  "c2e0c6" "3D exploration view"
create_label "area/colony"       "bfd4f2" "2D colony view"
create_label "area/caste"        "d4c5f9" "Caste system"
create_label "area/hazard"       "fef2c0" "Environmental hazards (rain, phorid)"
create_label "area/ui"           "fad8c7" "UI/UX"
create_label "area/procedural"   "c5f0e0" "Procedural placement systems"

# --- Status ---
create_label "blocked"       "000000" "Cannot proceed until dependency resolved"
create_label "needs-triage"  "ededed" "Not yet scoped/assigned to a milestone"

echo "Labels created."
