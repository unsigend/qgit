load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

init_repo() {
    "$QGIT_BIN" init -q
}

write_blob() {
    "$QGIT_BIN" hash-object -w "$1"
}

git_cat() {
    env GIT_OBJECT_DIRECTORY=.qgit/objects GIT_DIR=.qgit "$GIT" cat-file "$@"
}

assert_matches_git_cat() {
    local expected
    expected=$(git_cat "$@")
    run "$QGIT_BIN" cat-file "$@"
    assert_success
    assert_output_equals "$expected"
}

@test "qgit cat-file -p: matches git" {
    init_repo
    printf 'hello world\n' > file
    local hash
    hash=$(write_blob file)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: empty blob" {
    init_repo
    : > empty
    local hash
    hash=$(write_blob empty)
    run "$QGIT_BIN" cat-file -p "$hash"
    assert_success
    assert_output_empty
    run git_cat -p "$hash"
    assert_success
    assert_output_empty
}

@test "qgit cat-file -p: binary blob matches git" {
    init_repo
    printf '\x00\x01\x02\xff\xfe' > binary
    local hash
    hash=$(write_blob binary)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -t: matches git" {
    init_repo
    printf 'typed blob\n' > file
    local hash
    hash=$(write_blob file)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -s: matches git" {
    init_repo
    printf 'size check\n' > file
    local hash
    hash=$(write_blob file)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file blob: matches git" {
    init_repo
    printf 'raw blob\n' > file
    local hash
    hash=$(write_blob file)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file commit: type mismatch fails" {
    init_repo
    printf 'blob only\n' > file
    local hash
    hash=$(write_blob file)
    run "$QGIT_BIN" cat-file commit "$hash"
    assert_failure
}

@test "qgit cat-file -p: fails without object" {
    init_repo
    run "$QGIT_BIN" cat-file -p
    assert_failure
}

@test "qgit cat-file -t: fails without object" {
    init_repo
    run "$QGIT_BIN" cat-file -t
    assert_failure
}

@test "qgit cat-file -s: fails without object" {
    init_repo
    run "$QGIT_BIN" cat-file -s
    assert_failure
}

@test "qgit cat-file blob: fails without object" {
    init_repo
    run "$QGIT_BIN" cat-file blob
    assert_failure
}

@test "qgit cat-file: fails with invalid type" {
    init_repo
    printf 'data\n' > file
    local hash
    hash=$(write_blob file)
    run "$QGIT_BIN" cat-file invalid "$hash"
    assert_failure
}

@test "qgit cat-file -p: fails with non-existent sha1" {
    init_repo
    run "$QGIT_BIN" cat-file -p 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -p -t: fails together" {
    init_repo
    printf 'data\n' > file
    local hash
    hash=$(write_blob file)
    run "$QGIT_BIN" cat-file -p -t "$hash"
    assert_failure
}

@test "qgit cat-file -p -s: fails together" {
    init_repo
    printf 'data\n' > file
    local hash
    hash=$(write_blob file)
    run "$QGIT_BIN" cat-file -p -s "$hash"
    assert_failure
}

@test "qgit cat-file -s -t: fails together" {
    init_repo
    printf 'data\n' > file
    local hash
    hash=$(write_blob file)
    run "$QGIT_BIN" cat-file -s -t "$hash"
    assert_failure
}
