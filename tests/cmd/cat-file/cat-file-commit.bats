load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "../helpers/commit_messages.bash"
load "helpers/cat-file.bash"
load "helpers/cat-file-commit.bash"

# commit -p

@test "qgit cat-file -p: root commit matches git" {
    local hash
    hash=$(setup_root_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: commit with parent matches git" {
    local hash
    hash=$(setup_commit_with_parent)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: merge commit matches git" {
    local hash
    hash=$(setup_merge_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: multiline commit message matches git" {
    local hash
    hash=$(setup_multiline_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: empty commit message matches git" {
    local hash
    hash=$(setup_empty_message_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: different author and committer matches git" {
    local hash
    hash=$(setup_different_author_committer_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: non-UTC timezone matches git" {
    local hash
    hash=$(setup_timezone_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: utf8 author and message matches git" {
    local hash
    hash=$(setup_utf8_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: epoch timestamp matches git" {
    local hash
    hash=$(setup_epoch_timestamp_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: long parent chain matches git" {
    local hash
    hash=$(setup_long_chain_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: octopus merge matches git" {
    local hash
    hash=$(setup_octopus_merge_commit)
    assert_matches_git_cat_p "$hash"
}

# commit message edge cases

@test "qgit cat-file -p: message starting with author matches git" {
    local hash
    hash=$(setup_message_commit "author note only")
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: message starting with committer matches git" {
    local hash
    hash=$(setup_message_commit "committer note only")
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: message starting with tree matches git" {
    local hash
    hash=$(setup_message_commit "tree rebuild notes")
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: message starting with parent matches git" {
    local hash
    hash=$(setup_message_commit "parent linkage notes")
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file commit: message starting with tree matches git" {
    local hash
    hash=$(setup_message_commit "tree rebuild notes")
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: message starting with parent matches git" {
    local hash
    hash=$(setup_message_commit "parent linkage notes")
    assert_matches_git_cat commit "$hash"
}

# commit -t

@test "qgit cat-file -t: root commit type matches git" {
    local hash
    hash=$(setup_root_commit)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: merge commit type matches git" {
    local hash
    hash=$(setup_merge_commit)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: commit with parent type matches git" {
    local hash
    hash=$(setup_commit_with_parent)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: empty message commit type matches git" {
    local hash
    hash=$(setup_empty_message_commit)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: octopus merge type matches git" {
    local hash
    hash=$(setup_octopus_merge_commit)
    assert_matches_git_cat -t "$hash"
}

# commit -s

@test "qgit cat-file -s: root commit size matches git" {
    local hash
    hash=$(setup_root_commit)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: commit with parent size matches git" {
    local hash
    hash=$(setup_commit_with_parent)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: merge commit size matches git" {
    local hash
    hash=$(setup_merge_commit)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: empty message commit size matches git" {
    local hash
    hash=$(setup_empty_message_commit)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: multiline commit size matches git" {
    local hash
    hash=$(setup_multiline_commit)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: octopus merge size matches git" {
    local hash
    hash=$(setup_octopus_merge_commit)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: epoch timestamp commit size matches git" {
    local hash
    hash=$(setup_epoch_timestamp_commit)
    assert_matches_git_cat -s "$hash"
}

# commit raw

@test "qgit cat-file commit: root commit matches git" {
    local hash
    hash=$(setup_root_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: commit with parent matches git" {
    local hash
    hash=$(setup_commit_with_parent)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: merge commit matches git" {
    local hash
    hash=$(setup_merge_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: multiline commit message matches git" {
    local hash
    hash=$(setup_multiline_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: empty message matches git" {
    local hash
    hash=$(setup_empty_message_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: different author and committer matches git" {
    local hash
    hash=$(setup_different_author_committer_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: long parent chain matches git" {
    local hash
    hash=$(setup_long_chain_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: octopus merge matches git" {
    local hash
    hash=$(setup_octopus_merge_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: epoch timestamp matches git" {
    local hash
    hash=$(setup_epoch_timestamp_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: blob type on commit fails" {
    local hash
    hash=$(setup_root_commit)
    run_cat_file blob "$hash"
    assert_failure
}

@test "qgit cat-file commit: tag type on commit fails" {
    local hash
    hash=$(setup_root_commit)
    run_cat_file tag "$hash"
    assert_failure
}

@test "qgit cat-file commit: tree type on commit fails" {
    local hash
    hash=$(setup_root_commit)
    run_cat_file tree "$hash"
    assert_failure
}

# commit parse errors

@test "qgit cat-file -p: commit missing tree line fails" {
    local file="$TEST_DIR/commit-missing-tree.txt"

    write_commit_missing_tree "$file"
    assert_cat_file_p_rejects_malformed_commit "$file"
}

@test "qgit cat-file -p: commit missing author line fails" {
    local file="$TEST_DIR/commit-missing-author.txt"

    write_commit_missing_author "$file"
    assert_cat_file_p_rejects_malformed_commit "$file"
}

@test "qgit cat-file -p: commit missing committer line fails" {
    local file="$TEST_DIR/commit-missing-committer.txt"

    write_commit_missing_committer "$file"
    assert_cat_file_p_rejects_malformed_commit "$file"
}

@test "qgit cat-file -p: commit missing all header lines fails" {
    local file="$TEST_DIR/commit-missing-headers.txt"

    write_commit_missing_all_headers "$file"
    assert_cat_file_p_rejects_malformed_commit "$file"
}

@test "qgit cat-file -p: commit duplicate tree line fails" {
    local file="$TEST_DIR/commit-duplicate-tree.txt"

    write_commit_duplicate_tree "$file"
    assert_cat_file_p_rejects_malformed_commit "$file"
}

@test "qgit cat-file -p: commit duplicate author line fails" {
    local file="$TEST_DIR/commit-duplicate-author.txt"

    write_commit_duplicate_author "$file"
    assert_cat_file_p_rejects_malformed_commit "$file"
}

@test "qgit cat-file -p: commit duplicate committer line fails" {
    local file="$TEST_DIR/commit-duplicate-committer.txt"

    write_commit_duplicate_committer "$file"
    assert_cat_file_p_rejects_malformed_commit "$file"
}

@test "qgit cat-file -p: commit unknown header line fails" {
    local file="$TEST_DIR/commit-unknown-header.txt"

    write_commit_unknown_header "$file"
    assert_cat_file_p_rejects_malformed_commit "$file"
}

@test "qgit cat-file -p: commit bad author signature fails" {
    local file="$TEST_DIR/commit-bad-author.txt"

    write_commit_bad_author_signature "$file"
    assert_cat_file_p_rejects_malformed_commit "$file"
}

@test "qgit cat-file -p: commit bad committer signature fails" {
    local file="$TEST_DIR/commit-bad-committer.txt"

    write_commit_bad_committer_signature "$file"
    assert_cat_file_p_rejects_malformed_commit "$file"
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
