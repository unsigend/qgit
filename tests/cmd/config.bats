#!/usr/bin/env bats

load helper

setup() {
  cd "$BATS_TEST_TMPDIR" || return 1
  run "$QGIT" init repo
  cd repo
}

@test "config: --help exit status" {
  run "$QGIT" config --help
  [ "$status" -eq 0 ]
}

@test "config: -h exit status" {
  run "$QGIT" config -h
  [ "$status" -eq 0 ]
}

@test "config: unknown long option exit status" {
  run "$QGIT" config --this-option-does-not-exist
  [ "$status" -eq 1 ]
}

@test "config: no action exit status" {
  run "$QGIT" config
  [ "$status" -eq 1 ]
}

@test "config: --list --local has content" {
  run "$QGIT" config --list --local
  [ "$status" -eq 0 ]
  [ -n "$output" ]
}

@test "config: --list --local outside repo fails" {
  cd "$BATS_TEST_TMPDIR"
  run "$QGIT" config --list --local
  [ "$status" -eq 1 ]
}

@test "config: --list has content" {
  run "$QGIT" config --list
  [ "$status" -eq 0 ]
  [ -n "$output" ]
}
