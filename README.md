# Formicidae

An ant colony simulation built in Unreal Engine (C++), centered on a biologically-grounded pheromone system: ants leave the nest, explore, and gradually carve out trails through emergent, decaying pheromone deposits, not scripted pathfinding. The player acts as an additional pheromone source
rather than a direct controller, nudging exploration and rerouting traffic
without ever fully overriding the colony's own emergent behavior.

The project is built around two continuously-running, independently-updating
views: an **exploration view** (3D, ground-level, where trails form and
hazards threaten foragers) and a **colony view** (2D cross-section, where
resources become growth, caste allocation, and room construction). The two
views are connected by a single resource economy, what's found on the
surface funds what's built below.

This project is being developed as a professionalizing project (24 weeks,
part-time), documented under `docs/` alongside its development roadmap.

## Tech stack

- **Engine:** Unreal Engine 5, C++
- **Platform:** Linux (development), Desktop (target)
- **Version control:** Git + Git LFS

## Getting started

1. Clone the repo:
   ```bash
   git clone <repo-url>
   cd formicidae
   ```
2. Install local git hooks (commit message + branch name checks):
   ```bash
   ./scripts/install-hooks.sh
   ```
3. Open `formicidae.uproject` in Unreal Editor.

## Repository structure

```
formicidae/
├── Source/              # C++ source
├── Content/             # Unreal assets
├── Config/              # Engine/project configuration
├── .github/workflows/   # CI: commit/branch linting, backup mirror
├── .githooks/           # Local git hooks (see scripts/install-hooks.sh)
├── scripts/             # Repo setup scripts (labels, milestones, issues)
├── CONTRIBUTING.md      # Branching model, commit convention, PR rules
├── branch-protection.md # Required GitHub branch protection settings
└── mirror-setup.md      # Backup mirror setup instructions
```

## Contributing

See [CONTRIBUTING.md](./CONTRIBUTING.md) for the branching model, commit
message convention (Conventional Commits), and pull request rules. All
changes land on `main` via pull request — direct pushes are blocked.
