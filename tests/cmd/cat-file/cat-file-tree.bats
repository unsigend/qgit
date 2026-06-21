load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "helpers/cat-file.bash"
load "helpers/cat-file-tree.bash"

# tree -p

@test "qgit cat-file -p: single file tree matches git" {
    local hash
    hash=$(setup_single_file_tree)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: empty tree matches git" {
    local hash
    hash=$(setup_empty_tree)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: multi-entry tree matches git" {
    local hash
    hash=$(setup_multi_entry_tree)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: nested tree matches git" {
    local hash
    hash=$(setup_nested_tree)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: mixed file and subtree matches git" {
    local hash
    hash=$(setup_mixed_tree)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: executable entry tree matches git" {
    local hash
    hash=$(setup_executable_tree)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: symlink entry tree matches git" {
    local hash
    hash=$(setup_symlink_tree)
    assert_matches_git_cat_p "$hash"
}

# tree -t

@test "qgit cat-file -t: single file tree type matches git" {
    local hash
    hash=$(setup_single_file_tree)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: empty tree type matches git" {
    local hash
    hash=$(setup_empty_tree)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: multi-entry tree type matches git" {
    local hash
    hash=$(setup_multi_entry_tree)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: nested tree type matches git" {
    local hash
    hash=$(setup_nested_tree)
    assert_matches_git_cat -t "$hash"
}

# tree -s

@test "qgit cat-file -s: single file tree size matches git" {
    local hash
    hash=$(setup_single_file_tree)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: empty tree size matches git" {
    local hash
    hash=$(setup_empty_tree)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: multi-entry tree size matches git" {
    local hash
    hash=$(setup_multi_entry_tree)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: nested tree size matches git" {
    local hash
    hash=$(setup_nested_tree)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: mixed file and subtree size matches git" {
    local hash
    hash=$(setup_mixed_tree)
    assert_matches_git_cat -s "$hash"
}

# tree raw

@test "qgit cat-file tree: single file tree matches git" {
    local hash
    hash=$(setup_single_file_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: empty tree matches git" {
    local hash
    hash=$(setup_empty_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: multi-entry tree matches git" {
    local hash
    hash=$(setup_multi_entry_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: nested tree matches git" {
    local hash
    hash=$(setup_nested_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: mixed file and subtree matches git" {
    local hash
    hash=$(setup_mixed_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: executable entry tree matches git" {
    local hash
    hash=$(setup_executable_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: symlink entry tree matches git" {
    local hash
    hash=$(setup_symlink_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: blob type on tree fails" {
    local hash
    hash=$(setup_single_file_tree)
    run_cat_file blob "$hash"
    assert_failure
}

@test "qgit cat-file tree: commit type on tree fails" {
    local hash
    hash=$(setup_single_file_tree)
    run_cat_file commit "$hash"
    assert_failure
}

@test "qgit cat-file tree: tag type on tree fails" {
    local hash
    hash=$(setup_single_file_tree)
    run_cat_file tag "$hash"
    assert_failure
}

# tree errors

@test "qgit cat-file -p: missing tree object file fails" {
    setup_single_file_tree >/dev/null
    run_cat_file -p 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -t: missing tree object file fails" {
    setup_single_file_tree >/dev/null
    run_cat_file -t 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -s: missing tree object file fails" {
    setup_single_file_tree >/dev/null
    run_cat_file -s 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file tree: missing tree object file fails" {
    setup_single_file_tree >/dev/null
    run_cat_file tree 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -p -t: flags cannot combine on tree" {
    local hash
    hash=$(setup_single_file_tree)
    run_cat_file -p -t "$hash"
    assert_failure
}

@test "qgit cat-file -p -s: flags cannot combine on tree" {
    local hash
    hash=$(setup_single_file_tree)
    run_cat_file -p -s "$hash"
    assert_failure
}

@test "qgit cat-file -s -t: flags cannot combine on tree" {
    local hash
    hash=$(setup_single_file_tree)
    run_cat_file -s -t "$hash"
    assert_failure
}
