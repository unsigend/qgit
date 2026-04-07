#!/usr/bin/env bats

load helper

setup() {
  cd "$BATS_TEST_TMPDIR" || return 1
}

@test "init: no extra arguments" {
  mkdir empty && cd empty
  run "$QGIT" init
  st_qgit=$status
  [[ "$output" =~ Initialized.*/.qgit/$ ]]
  run "$GIT" init
  st_git=$status
  expect_ok_norm
}

@test "init: explicit ." {
  mkdir dot && cd dot
  run "$QGIT" init .
  st_qgit=$status
  [[ "$output" =~ Initialized.*/.qgit/$ ]]
  run "$GIT" init .
  st_git=$status
  expect_ok_norm
}

@test "init: --help exit status" {
  run "$QGIT" init --help
  [ "$status" -eq 0 ]
  run "$GIT" init --help
  [ "$status" -eq 0 ]
}

@test "init: -h exit status" {
  run "$QGIT" init -h
  [ "$status" -eq 0 ]
}

@test "init: help flags create no directory" {
  run "$QGIT" init --help
  [ ! -d .qgit ]
  run "$QGIT" init -h
  [ ! -d .qgit ]
  run "$GIT" init --help
  [ ! -d .git ]
}

@test "init: unknown long option exit status" {
  run "$QGIT" init --this-option-does-not-exist
  [ "$status" -eq 1 ]
}

@test "init: --quiet" {
  mkdir quiet && cd quiet
  run "$QGIT" init --quiet
  [ "$status" -eq 0 ]
  [ -d .qgit/objects ]
  [ -d .qgit/refs ]
  [ -d .qgit/refs/heads ]
  [ -d .qgit/refs/tags ]
  [ -f .qgit/HEAD ]
  [ -f .qgit/config ]
  [ -f .qgit/description ]
  cd "$BATS_TEST_TMPDIR"
  mkdir gquiet && cd gquiet
  run "$GIT" init --quiet
  [ "$status" -eq 0 ]
}

@test "init: -b sets initial branch" {
  run "$QGIT" init -b trunk qrepo
  [ "$status" -eq 0 ]
  [ -d qrepo/.qgit/objects ]
  [ -d qrepo/.qgit/refs ]
  [ -d qrepo/.qgit/refs/heads ]
  [ -d qrepo/.qgit/refs/tags ]
  [ -f qrepo/.qgit/HEAD ]
  [ -f qrepo/.qgit/config ]
  [ -f qrepo/.qgit/description ]
  run "$GIT" init -b trunk grepo
  [ "$status" -eq 0 ]
  [ "$(cat qrepo/.qgit/HEAD)" = "$(cat grepo/.git/HEAD)" ]
}

@test "init: --initial-branch sets initial branch" {
  run "$QGIT" init --initial-branch trunk qrepo
  [ "$status" -eq 0 ]
  [ -d qrepo/.qgit/objects ]
  [ -d qrepo/.qgit/refs ]
  [ -d qrepo/.qgit/refs/heads ]
  [ -d qrepo/.qgit/refs/tags ]
  [ -f qrepo/.qgit/HEAD ]
  [ -f qrepo/.qgit/config ]
  [ -f qrepo/.qgit/description ]
  run "$GIT" init --initial-branch trunk grepo
  [ "$status" -eq 0 ]
  [ "$(cat qrepo/.qgit/HEAD)" = "$(cat grepo/.git/HEAD)" ]
}

@test "init: specify dir" {
  run "$QGIT" init qrepo
  [ "$status" -eq 0 ]
  [ -d qrepo ]
  run "$GIT" init grepo
  [ "$status" -eq 0 ]
  [ -d grepo ]
}

@test "init: nested dir" {
  run "$QGIT" init "missing/parent/dir/repo"
  [ "$status" -eq 0 ]
  [ -d "missing/parent/dir/repo" ]
  run "$GIT" init "missing/parent/dir/repo2"
  [ "$status" -eq 0 ]
  [ -d "missing/parent/dir/repo2" ]
}

@test "init: defaultBranch from global config" {
  export HOME="$BATS_TEST_TMPDIR/home"
  mkdir -p "$HOME"
  "$QGIT" config --set --global init.defaultBranch custom >/dev/null
  run "$QGIT" init repo
  [ "$status" -eq 0 ]
  grep -q "ref: refs/heads/custom" repo/.qgit/HEAD
}

@test "init: multiple nested dir" {
  run "$QGIT" init a/b/c/d/qrepo
  [ "$status" -eq 0 ]
  [ -d a/b/c/d/qrepo ]
  run "$GIT" init a/b/c/d/grepo
  [ "$status" -eq 0 ]
  [ -d a/b/c/d/grepo ]
}