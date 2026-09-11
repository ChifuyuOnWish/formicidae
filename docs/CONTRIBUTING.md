# Contributing / Workflow Rules — Formicidae

Solo project, but run with the same discipline as a team repo — this document is
itself part of the professional deliverable for the school defenses.

---

## 1. Branching model

- **Install the local hooks once after cloning:** `./scripts/install-hooks.sh`
  — this enables local commit-message and branch-name checks (matching the
  rules below) before you even push. CI (`lint.yml`) enforces the same rules
  on every PR regardless, so nothing slips through if hooks aren't installed.
- **`main` is protected.** No direct pushes, ever — all changes land via pull
  request, even solo. This is enforced at the GitHub settings level (see
  `branch-protection.md`), not just by convention.
- Branch naming, by type of work:
  - `feat/<short-description>` — new feature or system (e.g. `feat/deposit-merge`)
  - `fix/<short-description>` — bug fix
  - `research/<short-description>` — experiments/spikes not meant to ship as-is
    (relevant for Étape 1's algorithm research work)
  - `docs/<short-description>` — documentation only
  - `chore/<short-description>` — tooling, CI, repo config
- Branch off `main`, open a PR back into `main` when the branch is ready.
- Delete the branch after merge (GitHub can do this automatically on merge).

## 2. Commit convention — Conventional Commits

Format: `<type>(<scope>): <short description>`

**Types:** `feat`, `fix`, `docs`, `refactor`, `perf`, `test`, `chore`, `research`

**Scope** — use the system the commit touches, e.g. `pheromone`, `colony`,
`caste`, `ui`, `ci`.

Examples:
```
feat(pheromone): implement deposit merge with position drift
fix(pheromone): correct decay applying twice per tick
research(pheromone): benchmark spatial hash vs linear scan at 500 agents
docs(readme): add build instructions
chore(ci): add GitLab mirror workflow
```

Keep commits scoped to one logical change — small, reviewable commits over one
giant commit per checkpoint.

## 3. Pull request rules

- **Every PR must reference the issue it closes**, using `Closes #<issue number>`
  in the PR description — this is what links issues to milestones automatically
  and keeps the defense record traceable back to specific work.
- **Self-review checklist before merging** (solo project, no second reviewer —
  this checklist is the substitute):
  - [ ] Code compiles clean, no new warnings introduced
  - [ ] Change matches the linked issue's scope (no silent scope creep)
  - [ ] Relevant `UPROPERTY`/tuning values exposed if this is a tunable system
  - [ ] No debug-only code left in (temporary logging, hardcoded test values)
- **Squash merge only** — keeps `main`'s history one commit per completed unit
  of work, readable at a glance during a defense walkthrough.
- No merging with failing CI (once CI exists) or unresolved review comments.

## 4. Issue → Milestone mapping

Every issue is assigned to the checkpoint (Étape) milestone it belongs to.
Milestone due dates are the defense dates. An issue left open past its
milestone's due date is a visible, honest signal of schedule slippage —
this is intentional, not something to hide by silently reassigning it.

## 5. Definition of done (per checkpoint)

A checkpoint's milestone is only closed when every issue in it is closed AND
the corresponding feature is demonstrable live in-editor — not just "code
compiles." This mirrors the actual defense bar.
