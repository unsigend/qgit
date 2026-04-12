#!/usr/bin/env bats

load helper

# Two repos: git creates loose blob objects, mirror objects/ into .qgit/objects/
# so qgit reads the same SHA-1s. Only blob objects are tested.

setup() {
  cd "$BATS_TEST_TMPDIR" || return 1
  mkdir -p cf-work && cd cf-work || return 1
  GDIR="$PWD/g"
  QDIR="$PWD/q"
  export GDIR QDIR

  "$GIT" init "$GDIR" >/dev/null
  "$QGIT" init "$QDIR" >/dev/null

  # Blobs: mix stdin and on-disk files, varied sizes (bytes)
  H1=$(cd "$GDIR" && printf 'A' | "$GIT" hash-object -w --stdin)
  H2=$(cd "$GDIR" && printf 'hello' | "$GIT" hash-object -w --stdin)
  H3=$(cd "$GDIR" && printf '%64s' '' | tr ' ' 'M' | "$GIT" hash-object -w --stdin)
  H4=$(cd "$GDIR" && printf '%400s' '' | tr ' ' 'W' | "$GIT" hash-object -w --stdin)
  H5=$(cd "$GDIR" && printf '' | "$GIT" hash-object -w --stdin)

  mkdir -p "$GDIR/src/nested"
  printf 'tiny\n' >"$GDIR/src/nested/note.txt"
  H6=$(cd "$GDIR" && "$GIT" hash-object -w src/nested/note.txt)

  printf '%128s' '' | tr ' ' 'P' >"$GDIR/padded.dat"
  H7=$(cd "$GDIR" && "$GIT" hash-object -w padded.dat)

  printf '%2048s' '' | tr ' ' 'Z' >"$GDIR/large.blob"
  H8=$(cd "$GDIR" && "$GIT" hash-object -w large.blob)

  printf 'line one\nline two\n' >"$GDIR/multiline.txt"
  H9=$(cd "$GDIR" && "$GIT" hash-object -w multiline.txt)

  export H1 H2 H3 H4 H5 H6 H7 H8 H9

  mkdir -p "$QDIR/.qgit/objects"
  cp -a "$GDIR/.git/objects/." "$QDIR/.qgit/objects/"

  cd "$QDIR" || return 1
}

@test "cat-file: -t blob matches git (multiple sizes)" {
  for h in "$H1" "$H2" "$H3" "$H4" "$H5" "$H6" "$H7" "$H8" "$H9"; do
    run "$GIT" --git-dir="$GDIR/.git" cat-file -t "$h"
    want_out=$output
    want_st=$status
    run "$QGIT" cat-file -t "$h"
    [ "$status" -eq "$want_st" ]
    [ "$output" = "$want_out" ]
  done
}

@test "cat-file: -s blob matches git (multiple sizes)" {
  for h in "$H1" "$H2" "$H3" "$H4" "$H5" "$H6" "$H7" "$H8" "$H9"; do
    run "$GIT" --git-dir="$GDIR/.git" cat-file -s "$h"
    want_out=$output
    want_st=$status
    run "$QGIT" cat-file -s "$h"
    [ "$status" -eq "$want_st" ]
    [ "$output" = "$want_out" ]
  done
}

@test "cat-file: -p blob matches git (multiple sizes)" {
  for h in "$H1" "$H2" "$H3" "$H4" "$H5" "$H6" "$H7" "$H8" "$H9"; do
    run "$GIT" --git-dir="$GDIR/.git" cat-file -p "$h"
    want_out=$output
    want_st=$status
    run "$QGIT" cat-file -p "$h"
    [ "$status" -eq "$want_st" ]
    [ "$output" = "$want_out" ]
  done
}

# qgit-only: exit status 1

@test "cat-file: -t and -s together exits 1" {
  run "$QGIT" cat-file -t -s deadbeefdeadbeefdeadbeefdeadbeefdeadbeef
  [ "$status" -eq 1 ]
  [[ "$output" == *"'-t' and '-s' cannot be used together"* ]]
}

@test "cat-file: -t and -p together exits 1" {
  run "$QGIT" cat-file -t -p deadbeefdeadbeefdeadbeefdeadbeefdeadbeef
  [ "$status" -eq 1 ]
  [[ "$output" == *"'-t' and '-p' cannot be used together"* ]]
}

@test "cat-file: -s and -p together exits 1" {
  run "$QGIT" cat-file -s -p deadbeefdeadbeefdeadbeefdeadbeefdeadbeef
  [ "$status" -eq 1 ]
  [[ "$output" == *"'-s' and '-p' cannot be used together"* ]]
}

@test "cat-file: three mode flags together exits 1" {
  run "$QGIT" cat-file -t -s -p deadbeefdeadbeefdeadbeefdeadbeefdeadbeef
  [ "$status" -eq 1 ]
  [[ "$output" == *"cannot be used together"* ]]
}

@test "cat-file: missing hash exits 1" {
  run "$QGIT" cat-file -t
  [ "$status" -eq 1 ]
  [[ "$output" == *"a hash is required"* ]]
}

@test "cat-file: outside repository exits 1" {
  mkdir -p "$BATS_TEST_TMPDIR/not-a-repo"
  cd "$BATS_TEST_TMPDIR/not-a-repo" || return 1
  run "$QGIT" cat-file -p deadbeefdeadbeefdeadbeefdeadbeefdeadbeef
  [ "$status" -eq 1 ]
  [[ "$output" == *"not a qgit repository"* ]]
}

@test "cat-file: unknown object hash exits 1" {
  run "$QGIT" cat-file -t deadbeefdeadbeefdeadbeefdeadbeefdeadbeef
  [ "$status" -eq 1 ]
}

@test "cat-file: invalid short hash exits 1" {
  run "$QGIT" cat-file -s abc
  [ "$status" -eq 1 ]
}
