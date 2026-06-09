load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

init_repo() {
    "$QGIT_BIN" init -q
}

assert_matches_git() {
    local expected
    expected=$("$GIT" hash-object "$@")
    run "$QGIT_BIN" hash-object "$@"
    assert_success
    assert_output_equals "$expected"
}

object_path() {
    local hash="$1"
    echo ".qgit/objects/${hash:0:2}/${hash:2}"
}

@test "qgit hash-object: matches git" {
    printf 'hello world\n' > file
    assert_matches_git file
}

@test "qgit hash-object: empty file matches git" {
    : > empty
    assert_matches_git empty
}

@test "qgit hash-object: binary file matches git" {
    printf '\x00\x01\x02\xff\xfe' > binary
    assert_matches_git binary
}

@test "qgit hash-object: large file matches git" {
    dd if=/dev/zero bs=1024 count=512 of=large 2>/dev/null
    assert_matches_git large
}

@test "qgit hash-object -t blob: matches git" {
    printf 'blob content\n' > file
    assert_matches_git -t blob file
}

@test "qgit hash-object -w: writes object matching git" {
    init_repo
    printf 'write me\n' > file
    local expected
    expected=$("$GIT" hash-object file)
    run "$QGIT_BIN" hash-object -w file
    assert_success
    assert_output_equals "$expected"
    assert_file_exists "$(object_path "$expected")"
}

@test "qgit hash-object -w: object content matches file" {
    init_repo
    printf 'stored payload' > file
    local hash
    hash=$("$QGIT_BIN" hash-object -w file)
    run env GIT_OBJECT_DIRECTORY=.qgit/objects GIT_DIR=.qgit \
        "$GIT" cat-file -p "$hash"
    assert_success
    assert_output_equals "$(cat file)"
}

@test "qgit hash-object -w: idempotent" {
    init_repo
    printf 'same content\n' > file
    local hash path before
    hash=$("$QGIT_BIN" hash-object -w file)
    path=$(object_path "$hash")
    before=$(cat "$path")
    run "$QGIT_BIN" hash-object -w file
    assert_success
    assert_output_equals "$hash"
    [ "$(cat "$path")" = "$before" ] || {
        echo "Expected object file unchanged"
        return 1
    }
}

@test "qgit hash-object -w: fails outside repo" {
    printf 'orphan\n' > file
    run "$QGIT_BIN" hash-object -w file
    assert_failure
    assert_output_contains "not inside a qgit repository"
}

@test "qgit hash-object: fails without file argument" {
    run "$QGIT_BIN" hash-object
    assert_failure
}

@test "qgit hash-object: fails when file does not exist" {
    run "$QGIT_BIN" hash-object missing-file
    assert_failure
}

@test "qgit hash-object --type: fails without value" {
    printf 'data\n' > file
    run "$QGIT_BIN" hash-object --type file
    assert_failure
}
