load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "helpers/cat-file.bash"
load "helpers/cat-file-blob.bash"

# blob -p

@test "qgit cat-file -p: text blob matches git" {
    printf 'hello world\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: empty blob matches git" {
    : >empty
    local hash
    hash=$(setup_blob_from_file empty)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: binary blob matches git" {
    printf '\x00\x01\x02\xff\xfe' >binary
    local hash
    hash=$(setup_blob_from_file binary)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: large blob matches git" {
    dd if=/dev/zero bs=1024 count=64 of=large 2>/dev/null
    local hash
    hash=$(setup_blob_from_file large)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: utf8 blob matches git" {
    printf 'café 日本語\n' >utf8
    local hash
    hash=$(setup_blob_from_file utf8)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: blob without trailing newline matches git" {
    printf 'no newline' >raw
    local hash
    hash=$(setup_blob_from_file raw)
    assert_matches_git_cat -p "$hash"
}

# blob -t

@test "qgit cat-file -t: blob type matches git" {
    printf 'typed blob\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: empty blob type matches git" {
    : >empty
    local hash
    hash=$(setup_blob_from_file empty)
    assert_matches_git_cat -t "$hash"
}

# blob -s

@test "qgit cat-file -s: blob size matches git" {
    printf 'size check\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: empty blob size matches git" {
    : >empty
    local hash
    hash=$(setup_blob_from_file empty)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: binary blob size matches git" {
    printf '\x00\x01\x02\xff\xfe' >binary
    local hash
    hash=$(setup_blob_from_file binary)
    assert_matches_git_cat -s "$hash"
}

# blob raw

@test "qgit cat-file blob: text blob matches git" {
    printf 'raw blob\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: empty blob matches git" {
    : >empty
    local hash
    hash=$(setup_blob_from_file empty)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: binary blob matches git" {
    printf '\x00\x01\x02\xff\xfe' >binary
    local hash
    hash=$(setup_blob_from_file binary)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: large blob matches git" {
    dd if=/dev/zero bs=1024 count=64 of=large 2>/dev/null
    local hash
    hash=$(setup_blob_from_file large)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: wrong type fails" {
    printf 'blob only\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file commit "$hash"
    assert_failure
}

@test "qgit cat-file blob: tree type on blob fails" {
    printf 'blob only\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file tree "$hash"
    assert_failure
}

@test "qgit cat-file blob: tag type on blob fails" {
    printf 'blob only\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file tag "$hash"
    assert_failure
}

# blob errors

@test "qgit cat-file -p: missing blob object file fails" {
    init_repo
    run_cat_file -p 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -t: missing blob object file fails" {
    init_repo
    run_cat_file -t 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -s: missing blob object file fails" {
    init_repo
    run_cat_file -s 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file blob: missing blob object file fails" {
    init_repo
    run_cat_file blob 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -p -t: flags cannot combine on blob" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file -p -t "$hash"
    assert_failure
}

@test "qgit cat-file -p -s: flags cannot combine on blob" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file -p -s "$hash"
    assert_failure
}

@test "qgit cat-file -s -t: flags cannot combine on blob" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file -s -t "$hash"
    assert_failure
}
