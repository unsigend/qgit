load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "helpers/cat-file.bash"
load "helpers/cat-file-blob.bash"
load "helpers/cat-file-commit.bash"

# name resolution

@test "qgit cat-file -t: HEAD resolves commit type" {
    setup_commit_with_branch_ref >/dev/null
    assert_matches_git_cat -t HEAD
}

@test "qgit cat-file -t: branch name resolves commit type" {
    setup_commit_with_branch_ref >/dev/null
    assert_matches_git_cat -t "$QGIT_DEFAULT_BRANCH"
}

@test "qgit cat-file -t: refs/heads path resolves commit type" {
    setup_commit_with_branch_ref >/dev/null
    assert_matches_git_cat -t "refs/heads/$QGIT_DEFAULT_BRANCH"
}

@test "qgit cat-file -t: short sha1 resolves commit type" {
    local sha abbrev

    sha=$(setup_commit_with_branch_ref)
    abbrev=$(abbrev_sha "$sha" 7)
    assert_matches_git_cat -t "$abbrev"
}

@test "qgit cat-file -p: HEAD resolves commit contents" {
    setup_commit_with_branch_ref >/dev/null
    assert_matches_git_cat_p HEAD
}

@test "qgit cat-file -t: tag name resolves commit type" {
    local sha

    sha=$(setup_commit_with_branch_ref)
    write_ref "refs/tags/v1.0" "$sha"
    assert_matches_git_cat -t v1.0
}

@test "qgit cat-file -p: unknown ref name fails" {
    setup_commit_with_branch_ref >/dev/null
    run_cat_file -p no-such-branch
    assert_failure
}

@test "qgit cat-file commit: HEAD resolves in raw mode" {
    setup_commit_with_branch_ref >/dev/null
    assert_matches_git_cat commit HEAD
}

@test "qgit cat-file commit: branch name resolves in raw mode" {
    setup_commit_with_branch_ref >/dev/null
    assert_matches_git_cat commit "$QGIT_DEFAULT_BRANCH"
}

@test "qgit cat-file commit: short sha1 resolves in raw mode" {
    local sha abbrev

    sha=$(setup_commit_with_branch_ref)
    abbrev=$(abbrev_sha "$sha" 7)
    assert_matches_git_cat commit "$abbrev"
}

@test "qgit cat-file blob: wrong type still fails with resolved ref name" {
    setup_commit_with_branch_ref >/dev/null
    run_cat_file blob HEAD
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

# commit errors

@test "qgit cat-file -p: missing commit object file fails" {
    setup_root_commit >/dev/null
    run_cat_file -p 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -t: missing commit object file fails" {
    setup_root_commit >/dev/null
    run_cat_file -t 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -s: missing commit object file fails" {
    setup_root_commit >/dev/null
    run_cat_file -s 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file commit: missing commit object file fails" {
    setup_root_commit >/dev/null
    run_cat_file commit 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -p -t: flags cannot combine on commit" {
    local hash
    hash=$(setup_root_commit)
    run_cat_file -p -t "$hash"
    assert_failure
}

@test "qgit cat-file -p -s: flags cannot combine on commit" {
    local hash
    hash=$(setup_root_commit)
    run_cat_file -p -s "$hash"
    assert_failure
}

@test "qgit cat-file -s -t: flags cannot combine on commit" {
    local hash
    hash=$(setup_root_commit)
    run_cat_file -s -t "$hash"
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

@test "qgit cat-file commit: missing object fails" {
    init_repo
    run_cat_file commit
    assert_failure
}

@test "qgit cat-file tree: missing object fails" {
    init_repo
    run_cat_file tree
    assert_failure
}

@test "qgit cat-file tag: missing object fails" {
    init_repo
    run_cat_file tag
    assert_failure
}

@test "qgit cat-file: invalid type fails" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file invalid "$hash"
    assert_failure
}

@test "qgit cat-file: invalid object hash fails" {
    init_repo
    run_cat_file -p not-a-valid-sha
    assert_failure
}

@test "qgit cat-file: outside repository fails" {
    mkdir outside && cd outside || return 1
    run_cat_file -p deadbeef
    assert_failure
}

@test "qgit cat-file -p -t: flags cannot combine" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file -p -t "$hash"
    assert_failure
}

@test "qgit cat-file -p -s: flags cannot combine" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file -p -s "$hash"
    assert_failure
}

@test "qgit cat-file -s -t: flags cannot combine" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
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
