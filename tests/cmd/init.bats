#!/usr/bin/env bats

load helper

setup() {
  cd "$BATS_TEST_TMPDIR" || return 1
}

norm_brand() {
  printf '%s' "$1" | perl -pe 's/\b[Gg]it\b/TOOL/g; s/\bqgit\b/TOOL/gi'
}

expect_ok_norm() {
  [ "$st_git" -eq 0 ]
  [ "$st_qgit" -eq 0 ]
  [ "$(norm_brand "$1")" = "$(norm_brand "$2")" ]
}

@test "init: no extra arguments" {
  mkdir empty && cd empty
  run "$GIT" init
  st_git=$status
  og=$output
  run "$QGIT" init
  st_qgit=$status
  oq=$output
  expect_ok_norm "$og" "$oq"
}

@test "init: invalid path" {
  run "$QGIT" init "missing/parent/dir/repo"
  st_qgit=$status
  [ "$st_qgit" -eq 1 ]
}

@test "init: explicit ." {
  mkdir dot && cd dot
  run "$GIT" init .
  st_git=$status
  og=$output
  run "$QGIT" init .
  st_qgit=$status
  oq=$output
  expect_ok_norm "$og" "$oq"
}

@test "init: --help exit status" {
  run "$GIT" init --help
  st_git=$status
  run "$QGIT" init --help
  st_qgit=$status
  [ "$st_git" -eq 0 ]
  [ "$st_qgit" -eq 0 ]
}

@test "init: -h exit status" {
  run "$QGIT" init -h
  st_qgit=$status
  [ "$st_qgit" -eq 0 ]
}

@test "init: unknown long option exit status" {
  run "$QGIT" init --this-option-does-not-exist
  st_qgit=$status
  [ "$st_qgit" -eq 1 ]
}

@test "init: --quiet exit status" {
  mkdir quiet && cd quiet
  run "$GIT" init --quiet
  st_git=$status
  run "$QGIT" init --quiet
  st_qgit=$status
  [ "$st_git" -eq 0 ]
  [ "$st_qgit" -eq 0 ]
}
