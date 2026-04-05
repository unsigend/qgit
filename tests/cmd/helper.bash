# Shared functions for cmd/*.bats

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

norm_brand() {
  printf '%s' "$1" | perl -pe 's/\b[Gg]it\b/TOOL/g; s/\bqgit\b/TOOL/gi'
}

expect_ok_norm() {
  [ "$st_git" -eq 0 ]
  [ "$st_qgit" -eq 0 ]
  [ "$(norm_brand "$1")" = "$(norm_brand "$2")" ]
}
