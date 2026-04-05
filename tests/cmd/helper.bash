# Shared setup for cmd/*.bats

setup_file() {
  GIT="${GIT:-git}"
  export GIT
  if ! command -v "$GIT" >/dev/null 2>&1; then
    echo "git is required (set GIT to the git binary if not on PATH)" >&2
    return 1
  fi
  if [[ -z "${QGIT:-}" || ! -x "$QGIT" ]]; then
    echo "QGIT must point at the built qgit binary (run: make -C tests test)" >&2
    return 1
  fi
}
