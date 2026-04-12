#!/usr/bin/env bats

load helper

bats_require_minimum_version 1.5.0

# Mirrored trees: same bytes at the same relative paths under g/ (git) and q/ (qgit).
#
# Reference: compare qgit to git (hash line; cat-file after -w). We use
# `run --separate-stderr` so git noise on stderr never pollutes $output (stdout only).
#
# Paths are non-empty files only. A zero-length blob requires object_open to handle
# st_size==0 (e.g. mmap(0) is invalid); until then, do not add an empty file to the
# loops below — a mismatch there would be qgit/object_open behavior, not the harness.

setup() {
  cd "$BATS_TEST_TMPDIR" || return 1
  mkdir -p ho-work && cd ho-work || return 1
  GDIR="$PWD/g"
  QDIR="$PWD/q"
  export GDIR QDIR GIT QGIT

  "$GIT" init "$GDIR" >/dev/null 2>&1
  "$QGIT" init "$QDIR" >/dev/null

  printf 'A' >"$GDIR/f1"
  cp "$GDIR/f1" "$QDIR/f1"

  printf 'hello' >"$GDIR/f2"
  cp "$GDIR/f2" "$QDIR/f2"

  printf '%64s' '' | tr ' ' 'M' >"$GDIR/f3"
  cp "$GDIR/f3" "$QDIR/f3"

  printf '%400s' '' | tr ' ' 'W' >"$GDIR/f4"
  cp "$GDIR/f4" "$QDIR/f4"

  mkdir -p "$GDIR/src/nested" "$QDIR/src/nested"
  printf 'tiny\n' >"$GDIR/src/nested/note.txt"
  cp "$GDIR/src/nested/note.txt" "$QDIR/src/nested/note.txt"

  printf '%128s' '' | tr ' ' 'P' >"$GDIR/padded.dat"
  cp "$GDIR/padded.dat" "$QDIR/padded.dat"

  printf '%2048s' '' | tr ' ' 'Z' >"$GDIR/large.blob"
  cp "$GDIR/large.blob" "$QDIR/large.blob"

  printf 'line one\nline two\n' >"$GDIR/multiline.txt"
  cp "$GDIR/multiline.txt" "$QDIR/multiline.txt"

  cd "$QDIR" || return 1
}

@test "hash-object: printed hash matches git without -w (multiple files)" {
  for path in f1 f2 f3 f4 src/nested/note.txt padded.dat large.blob multiline.txt; do
    run --separate-stderr bash -ec '"$1" -C "$2" hash-object "$3"' bash "$GIT" "$GDIR" "$path"
    want_out=$output
    run --separate-stderr bash -ec 'cd "$1" && "$2" hash-object "$3"' bash "$QDIR" "$QGIT" "$path"
    [ "$output" = "$want_out" ]
  done
}

@test "hash-object: -w stores blob; cat-file -t/-s/-p match git (multiple files)" {
  for path in f1 f2 f3 f4 src/nested/note.txt padded.dat large.blob multiline.txt; do
    run --separate-stderr bash -ec 'cd "$1" && "$2" hash-object -w "$3"' bash "$GDIR" "$GIT" "$path"
    want_git=$output
    run --separate-stderr bash -ec 'cd "$1" && "$2" hash-object -w "$3"' bash "$QDIR" "$QGIT" "$path"
    [ "$output" = "$want_git" ]

    h=$(printf '%s' "$output" | tr -d '\r\n')

    run --separate-stderr bash -ec '"$1" -C "$2" cat-file -t "$3"' bash "$GIT" "$GDIR" "$h"
    want_out=$output
    run --separate-stderr bash -ec 'cd "$1" && "$2" cat-file -t "$3"' bash "$QDIR" "$QGIT" "$h"
    [ "$output" = "$want_out" ]

    run --separate-stderr bash -ec '"$1" -C "$2" cat-file -s "$3"' bash "$GIT" "$GDIR" "$h"
    want_out=$output
    run --separate-stderr bash -ec 'cd "$1" && "$2" cat-file -s "$3"' bash "$QDIR" "$QGIT" "$h"
    [ "$output" = "$want_out" ]

    run --separate-stderr bash -ec '"$1" -C "$2" cat-file -p "$3"' bash "$GIT" "$GDIR" "$h"
    want_out=$output
    run --separate-stderr bash -ec 'cd "$1" && "$2" cat-file -p "$3"' bash "$QDIR" "$QGIT" "$h"
    [ "$output" = "$want_out" ]
  done
}

@test "hash-object: without -w does not create loose object in .qgit/objects" {
  printf 'not-stored-by-hash-object\n' >"$QDIR/ephemeral.txt"
  run --separate-stderr bash -ec 'cd "$1" && "$2" hash-object "$3"' bash "$QDIR" "$QGIT" ephemeral.txt
  h=$(printf '%s' "$output" | tr -d '\r\n')
  [ "${#h}" -eq 40 ]
  obj="$QDIR/.qgit/objects/${h:0:2}/${h:2}"
  [ ! -e "$obj" ]
}

@test "hash-object: missing file argument exits 1" {
  run "$QGIT" hash-object
  [ "$status" -eq 1 ]
  [[ "$output" == *"file is required"* ]]
}


@test "hash-object: nonexistent file exits 1" {
  run --separate-stderr "$QGIT" hash-object "missing-$$.txt"
  [ "$status" -eq 1 ]
  [ -z "$output" ]
}

@test "hash-object: -w with no qgit repository exits 1" {
  mkdir -p "$BATS_TEST_TMPDIR/no-qgit"
  printf 'w' >"$BATS_TEST_TMPDIR/no-qgit/only-file"
  cd "$BATS_TEST_TMPDIR/no-qgit" || return 1
  run --separate-stderr "$QGIT" hash-object -w only-file
  [ "$status" -eq 1 ]
}
