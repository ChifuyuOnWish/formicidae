#!/usr/bin/env bash
# Creates the 12 bi-weekly milestones (Étapes) via GitHub CLI.
# Due dates are fixed to your actual defense schedule: every 2 weeks, on a
# Friday, starting 2026-09-25.
#
# Requires: gh CLI installed and authenticated, run from inside the repo.
set -euo pipefail

# ---- Fixed defense schedule ----
# Étape 1 due 2026-09-25 (Friday). Each subsequent Étape is due 2 weeks later.
# Since 14 days is exactly 2 weeks, every date below lands on a Friday too.
START_DATE="2026-09-25"
DUE_TIME="18:00:00" # adjust to your actual defense time if needed (local time)
# ---------------------------------

REPO="${1:-}"  # optional: owner/repo. Defaults to current repo.
REPO_FLAG=()
if [[ -n "$REPO" ]]; then
  REPO_FLAG=(--repo "$REPO")
fi

declare -a TITLES=(
  "Checkpoint 1 — Algorithm Research & Architecture"
  "Checkpoint 2 — Core Deposit System"
  "Checkpoint 3 — Single Ant Agent"
  "Checkpoint 4 — Swarm at Scale"
  "Checkpoint 5 — Player Pheromone Tools"
  "Checkpoint 6 — Scouting + First Hazard"
  "Checkpoint 7 — Colony View Foundation"
  "Checkpoint 8 — Caste System"
  "Checkpoint 9 — Full Loop Integration"
  "Checkpoint 10 — Should-Tier Scope"
  "Checkpoint 11 — Bonus / Polish"
  "Checkpoint 12 — Final Defense"
)

for i in "${!TITLES[@]}"; do
  weeks=$((i * 2))  # 0-indexed: Étape 1 = START_DATE + 0 weeks
  due_on=$(date -u -d "${START_DATE} +${weeks} weeks ${DUE_TIME}" +"%Y-%m-%dT%H:%M:%SZ")
  title="${TITLES[$i]}"

  gh api \
    --method POST \
    "repos/$(gh repo view "${REPO_FLAG[@]}" --json nameWithOwner -q .nameWithOwner)/milestones" \
    -f title="$title" \
    -f due_on="$due_on" \
    -f state="open" \
    >/dev/null

  echo "Created: $title (due $due_on UTC)"
done

echo "All 12 milestones created."