#!/usr/bin/env bats

load helper

setup() {
  export HOME="$BATS_TEST_TMPDIR/home"
  mkdir -p "$HOME"
  cd "$BATS_TEST_TMPDIR" || return 1
  "$QGIT" init repo >/dev/null
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

@test "config: unknown option fails" {
  run "$QGIT" config --this-option-does-not-exist
  [ "$status" -eq 1 ]
}

@test "config: no action fails" {
  run "$QGIT" config
  [ "$status" -eq 1 ]
}

@test "config: --set requires two arguments" {
  run "$QGIT" config --set
  [ "$status" -eq 1 ]
}

@test "config: --set requires section.key format" {
  run "$QGIT" config --set nokey value
  [ "$status" -eq 1 ]
}

@test "config: --get requires one argument" {
  run "$QGIT" config --get
  [ "$status" -eq 1 ]
}

@test "config: --get requires section.key format" {
  run "$QGIT" config --get nokey
  [ "$status" -eq 1 ]
}

@test "config: --unset requires one argument" {
  run "$QGIT" config --unset
  [ "$status" -eq 1 ]
}

@test "config: --unset requires section.key format" {
  run "$QGIT" config --unset nokey
  [ "$status" -eq 1 ]
}

# list

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

@test "config: --list --global with no global file exits ok silently" {
  run "$QGIT" config --list --global
  [ "$status" -eq 0 ]
  [ -z "$output" ]
}

@test "config: --list --global shows global content" {
  printf '[user]\n\tname = Tester\n' > "$HOME/.qgitconfig"
  run "$QGIT" config --list --global
  [ "$status" -eq 0 ]
  [ -n "$output" ]
}

@test "config: --list auto inside repo shows local content" {
  run "$QGIT" config --list
  [ "$status" -eq 0 ]
  [ -n "$output" ]
}

@test "config: --list auto outside repo exits ok silently" {
  cd "$BATS_TEST_TMPDIR"
  run "$QGIT" config --list
  [ "$status" -eq 0 ]
  [ -z "$output" ]
}

@test "config: --list auto shows global then local" {
  printf '[user]\n\tname = Tester\n' > "$HOME/.qgitconfig"
  run "$QGIT" config --list
  [ "$status" -eq 0 ]
  [[ "$output" == *"user.name=Tester"* ]]
  [[ "$output" == *"core."* ]]
}

# set

@test "config: --set writes local key" {
  run "$QGIT" config --set test.foo bar
  [ "$status" -eq 0 ]
  run "$QGIT" config --get test.foo
  [ "$status" -eq 0 ]
  [ "$output" = "bar" ]
}

@test "config: --set --local writes local key" {
  run "$QGIT" config --set --local test.foo baz
  [ "$status" -eq 0 ]
  run "$QGIT" config --get --local test.foo
  [ "$status" -eq 0 ]
  [ "$output" = "baz" ]
}

@test "config: --set --local key persists in config file" {
  run "$QGIT" config --set --local test.foo persisted
  [ "$status" -eq 0 ]
  grep -q "foo" .qgit/config
}

@test "config: --set --global writes global key" {
  run "$QGIT" config --set --global user.name "Test User"
  [ "$status" -eq 0 ]
  run "$QGIT" config --get --global user.name
  [ "$status" -eq 0 ]
  [ "$output" = "Test User" ]
}

@test "config: --set --global key persists in global file" {
  run "$QGIT" config --set --global user.name "Test User"
  [ "$status" -eq 0 ]
  [ -f "$HOME/.qgitconfig" ]
  grep -q "Test User" "$HOME/.qgitconfig"
}

@test "config: --set overwrites existing value" {
  run "$QGIT" config --set test.foo first
  run "$QGIT" config --set test.foo second
  [ "$status" -eq 0 ]
  run "$QGIT" config --get test.foo
  [ "$output" = "second" ]
}

@test "config: --set outside repo without --global fails" {
  cd "$BATS_TEST_TMPDIR"
  run "$QGIT" config --set test.foo bar
  [ "$status" -eq 1 ]
}

#get

@test "config: --get existing local key" {
  run "$QGIT" config --get core.repositoryformatversion
  [ "$status" -eq 0 ]
  [ "$output" = "0" ]
}

@test "config: --get --local existing key" {
  run "$QGIT" config --get --local core.repositoryformatversion
  [ "$status" -eq 0 ]
  [ "$output" = "0" ]
}

@test "config: --get nonexistent key exits 1 with no output" {
  run "$QGIT" config --get test.doesnotexist
  [ "$status" -eq 1 ]
  [ -z "$output" ]
}

@test "config: --get --local key not in local exits 1" {
  printf '[user]\n\tname = Tester\n' > "$HOME/.qgitconfig"
  run "$QGIT" config --get --local user.name
  [ "$status" -eq 1 ]
}

@test "config: --get --global existing key" {
  printf '[user]\n\tname = Tester\n' > "$HOME/.qgitconfig"
  run "$QGIT" config --get --global user.name
  [ "$status" -eq 0 ]
  [ "$output" = "Tester" ]
}

@test "config: --get auto local overrides global" {
  printf '[test]\n\tfoo = global\n' > "$HOME/.qgitconfig"
  "$QGIT" config --set --local test.foo local >/dev/null
  run "$QGIT" config --get test.foo
  [ "$status" -eq 0 ]
  [ "$output" = "local" ]
}

@test "config: --get auto falls back to global when not in local" {
  printf '[user]\n\tname = Tester\n' > "$HOME/.qgitconfig"
  run "$QGIT" config --get user.name
  [ "$status" -eq 0 ]
  [ "$output" = "Tester" ]
}

# unset

@test "config: --unset local key" {
  "$QGIT" config --set test.foo bar >/dev/null
  run "$QGIT" config --unset test.foo
  [ "$status" -eq 0 ]
  run "$QGIT" config --get test.foo
  [ "$status" -eq 1 ]
}

@test "config: --unset --local key" {
  "$QGIT" config --set --local test.foo bar >/dev/null
  run "$QGIT" config --unset --local test.foo
  [ "$status" -eq 0 ]
  run "$QGIT" config --get --local test.foo
  [ "$status" -eq 1 ]
}

@test "config: --unset --global key" {
  printf '[user]\n\tname = Tester\n' > "$HOME/.qgitconfig"
  run "$QGIT" config --unset --global user.name
  [ "$status" -eq 0 ]
  run "$QGIT" config --get --global user.name
  [ "$status" -eq 1 ]
}

@test "config: --unset outside repo fails" {
  cd "$BATS_TEST_TMPDIR"
  run "$QGIT" config --unset test.foo
  [ "$status" -eq 1 ]
}
