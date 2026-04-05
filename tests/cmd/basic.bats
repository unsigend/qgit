#!/usr/bin/env bats

load helper

UNKNOWN=unknown_subcommand

@test "no subcommand" {
  run "$GIT"
  st_git=$status
  run "$QGIT"
  st_qgit=$status
  [ "$st_git" -eq "$st_qgit" ]
}

@test "unknown subcommand" {
  run "$GIT" "$UNKNOWN"
  st_git=$status
  run "$QGIT" "$UNKNOWN"
  st_qgit=$status
  [ "$st_git" -eq "$st_qgit" ]
}

@test "--help" {
  run "$GIT" --help
  st_git=$status
  run "$QGIT" --help
  st_qgit=$status
  [ "$st_git" -eq "$st_qgit" ]
}

@test "-h" {
  run "$GIT" -h
  st_git=$status
  run "$QGIT" -h
  st_qgit=$status
  [ "$st_git" -eq "$st_qgit" ]
}
