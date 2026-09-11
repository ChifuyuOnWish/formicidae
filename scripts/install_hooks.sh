#!/usr/bin/env bash
# One-time setup: points git at the tracked .githooks/ directory instead of
# the default (untracked, per-clone) .git/hooks/. Run this once after
# cloning the repo.
set -euo pipefail

chmod +x .githooks/commit-msg .githooks/pre-push
git config core.hooksPath .githooks

echo "Hooks installed. core.hooksPath -> .githooks"
echo "Commit messages and branch names will now be checked locally."
