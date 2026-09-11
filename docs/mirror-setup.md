# Backup Mirror Setup (GitHub → GitHub)

`.github/workflows/mirror.yml` pushes a full mirror of this repo to a second
GitHub repository on every push. Since that second repo doesn't exist yet,
create it first.

## 1. Create the backup repo
Either on github.com (**New repository**, name it e.g. `formicidae-backup`,
leave it **completely empty** — no README, no `.gitignore`, no license: any
auto-generated file creates diverging history that rejects the first mirror
push) or via CLI:

```bash
gh repo create yourusername/formicidae-backup --private --confirm
```

(`--private` recommended since it's just a backup, not a public presence.)

## 2. Create a GitHub personal access token
GitHub → **Settings → Developer settings → Personal access tokens → Fine-grained
tokens** → generate one scoped specifically to the backup repo, with
**Contents: Read and write** permission. Copy it immediately.

(A classic token with the `repo` scope also works if you'd rather not scope it
per-repo, but fine-grained is the more locked-down option.)

## 3. Add repo secrets on the SOURCE repo (formicidae)
**Settings → Secrets and variables → Actions → New repository secret**:

- `MIRROR_TOKEN` — the token from step 2
- `MIRROR_REPO_PATH` — the backup repo path, e.g. `yourusername/formicidae-backup`

## 4. Confirm
Push any commit to `formicidae` — the `Mirror to backup repo` workflow should
appear under the **Actions** tab and complete successfully. Check the backup
repo; it should now match the source exactly, including all branches and tags.

## Notes
- Only `main` and tags are mirrored — research branches (`research/*`) and any
  other in-progress branches stay in the source repo only and never reach the
  backup. This lets a single repo hold both dev and research work without a
  third repo, since the evaluated copy only ever reflects reviewed, merged
  `main` history.
- One-way push (`main:main --force`). Never commit directly on the backup repo.
- Git LFS objects aren't included by a plain push — if that becomes relevant
  once LFS content grows, add `git lfs fetch --all` and
  `git lfs push backup main` as extra steps in the workflow.
- Until the backup repo exists and the secrets are set, this workflow will run
  and fail on every push to `main` (harmless, just a red X in the Actions tab)
  — either disable the workflow file until you're ready, or just ignore the
  failures short-term.
