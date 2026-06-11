load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

# helpers

init_repo() {
    "$QGIT_BIN" init -q
}

write_blob() {
    "$QGIT_BIN" hash-object -w "$1"
}

run_cat_file() {
    run "$QGIT_BIN" cat-file "$@"
}

git_cat() {
    env GIT_OBJECT_DIRECTORY=.qgit/objects GIT_DIR=.qgit "$GIT" cat-file "$@"
}

assert_matches_git_cat() {
    local expected
    expected=$(git_cat "$@")
    run_cat_file "$@"
    assert_success
    assert_output_equals "$expected"
}

# blob -p

@test "qgit cat-file -p: text blob matches git" {
    init_repo
    printf 'hello world\n' > file
    local hash
    hash=$(write_blob file)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: empty blob matches git" {
    init_repo
    : > empty
    local hash
    hash=$(write_blob empty)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: binary blob matches git" {
    init_repo
    printf '\x00\x01\x02\xff\xfe' > binary
    local hash
    hash=$(write_blob binary)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: large blob matches git" {
    init_repo
    dd if=/dev/zero bs=1024 count=64 of=large 2>/dev/null
    local hash
    hash=$(write_blob large)
    assert_matches_git_cat -p "$hash"
}

# blob -t

@test "qgit cat-file -t: blob type matches git" {
    init_repo
    printf 'typed blob\n' > file
    local hash
    hash=$(write_blob file)
    assert_matches_git_cat -t "$hash"
}

# blob -s

@test "qgit cat-file -s: blob size matches git" {
    init_repo
    printf 'size check\n' > file
    local hash
    hash=$(write_blob file)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: empty blob size matches git" {
    init_repo
    : > empty
    local hash
    hash=$(write_blob empty)
    assert_matches_git_cat -s "$hash"
}

# blob raw

@test "qgit cat-file blob: text blob matches git" {
    init_repo
    printf 'raw blob\n' > file
    local hash
    hash=$(write_blob file)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: empty blob matches git" {
    init_repo
    : > empty
    local hash
    hash=$(write_blob empty)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: binary blob matches git" {
    init_repo
    printf '\x00\x01\x02\xff\xfe' > binary
    local hash
    hash=$(write_blob binary)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: wrong type fails" {
    init_repo
    printf 'blob only\n' > file
    local hash
    hash=$(write_blob file)
    run_cat_file commit "$hash"
    assert_failure
}

@test "qgit cat-file blob: tree type on blob fails" {
    init_repo
    printf 'blob only\n' > file
    local hash
    hash=$(write_blob file)
    run_cat_file tree "$hash"
    assert_failure
}

# errors

@test "qgit cat-file -p: missing object fails" {
    init_repo
    run_cat_file -p
    assert_failure
}

@test "qgit cat-file -t: missing object fails" {
    init_repo
    run_cat_file -t
    assert_failure
}

@test "qgit cat-file -s: missing object fails" {
    init_repo
    run_cat_file -s
    assert_failure
}

@test "qgit cat-file blob: missing object fails" {
    init_repo
    run_cat_file blob
    assert_failure
}

@test "qgit cat-file: invalid type fails" {
    init_repo
    printf 'data\n' > file
    local hash
    hash=$(write_blob file)
    run_cat_file invalid "$hash"
    assert_failure
}

@test "qgit cat-file -p: missing object file fails" {
    init_repo
    run_cat_file -p 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file blob: missing object file fails" {
    init_repo
    run_cat_file blob 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file: outside repository fails" {
    mkdir outside && cd outside || return 1
    run_cat_file -p deadbeef
    assert_failure
}

@test "qgit cat-file -p -t: flags cannot combine" {
    init_repo
    printf 'data\n' > file
    local hash
    hash=$(write_blob file)
    run_cat_file -p -t "$hash"
    assert_failure
}

@test "qgit cat-file -p -s: flags cannot combine" {
    init_repo
    printf 'data\n' > file
    local hash
    hash=$(write_blob file)
    run_cat_file -p -s "$hash"
    assert_failure
}

@test "qgit cat-file -s -t: flags cannot combine" {
    init_repo
    printf 'data\n' > file
    local hash
    hash=$(write_blob file)
    run_cat_file -s -t "$hash"
    assert_failure
}

# -h

@test "qgit cat-file -h: shows help" {
    run_cat_file -h
    assert_success
    assert_output_contains "qgit cat-file"
    assert_output_contains "-p"
}

@test "qgit cat-file --help: shows help" {
    run_cat_file --help
    assert_success
    assert_output_contains "qgit cat-file"
    assert_output_contains "-s"
}
