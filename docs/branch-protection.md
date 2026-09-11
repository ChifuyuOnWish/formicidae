# Branch Protection Settings — `main`

Configure under **Settings → Branches → Add branch protection rule** for `main`.

## Recommended settings

- ✅ **Require a pull request before merging**
  - ✅ Require approvals: set to 0 for solo (GitHub won't let you require your own
    approval), but keep the PR requirement itself on — it still blocks direct
    pushes and forces the self-review checklist step in `CONTRIBUTING.md`.
- ✅ **Require conversation resolution before merging**
- ✅ **Do not allow bypassing the above settings** — applies the rule to you too,
  not just hypothetical collaborators. This is the actual mechanism that
  prevents "just this once" direct pushes to `main`.
- ✅ **Block force pushes**
- ✅ **Restrict deletions** (protects `main` from accidental deletion)
- Once CI exists (build verification): ✅ **Require status checks to pass before
  merging**, select the relevant workflow.

## Equivalent via `gh` CLI

If you'd rather script this than click through settings:

```bash
gh api \
  --method PUT \
  repos/<owner>/formicidae/branches/main/protection \
  -f required_status_checks='null' \
  -F enforce_admins=true \
  -f required_pull_request_reviews='{"required_approving_review_count":0}' \
  -f restrictions='null' \
  -F allow_force_pushes=false \
  -F allow_deletions=false
```

Replace `<owner>` with your GitHub username. Run this once after the repo exists
and has at least one commit on `main`.
