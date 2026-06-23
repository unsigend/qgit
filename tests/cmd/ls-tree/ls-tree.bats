load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "helpers/ls-tree.bash"

# ls-tree

@test "qgit ls-tree HEAD: tip commit matches git" {
    setup_single_file_commit >/dev/null
    assert_matches_git_ls_tree HEAD
}

@test "qgit ls-tree HEAD: linear history matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_ls_tree HEAD
}

@test "qgit ls-tree HEAD: explicit HEAD matches git" {
    setup_single_file_commit >/dev/null
    assert_matches_git_ls_tree HEAD
}

@test "qgit ls-tree HEAD: three commit history matches git" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_ls_tree HEAD
}

# <commit>

@test "qgit ls-tree <commit>: tip commit matches git" {
    setup_single_file_commit >/dev/null
    local tip
    tip=$(commit_at 0)
    assert_matches_git_ls_tree "$tip"
}

@test "qgit ls-tree <commit>: root commit matches git" {
    setup_linear_history 4 >/dev/null
    local root
    root=$(commit_at 0)
    assert_matches_git_ls_tree "$root"
}

@test "qgit ls-tree <commit>: middle commit matches git" {
    setup_linear_history 5 >/dev/null
    local mid
    mid=$(commit_at 2)
    assert_matches_git_ls_tree "$mid"
}

@test "qgit ls-tree <commit>: merge tip matches git" {
    setup_merge_tip_commit >/dev/null
    local merge_tip
    merge_tip=$(commit_at 2)
    assert_matches_git_ls_tree "$merge_tip"
}

@test "qgit ls-tree <commit>: empty tree commit matches git" {
    setup_empty_tree_commit >/dev/null
    local commit_sha
    commit_sha=$(commit_at 0)
    assert_matches_git_ls_tree "$commit_sha"
}

# <tree>

@test "qgit ls-tree <tree>: direct tree sha matches git" {
    setup_single_file_commit >/dev/null
    local commit_sha tree_sha
    commit_sha=$(commit_at 0)
    tree_sha=$(commit_tree_sha "$commit_sha")
    assert_matches_git_ls_tree "$tree_sha"
}

@test "qgit ls-tree <tree>: multi-entry tree matches git" {
    setup_multi_entry_tree_commit >/dev/null
    local commit_sha tree_sha
    commit_sha=$(commit_at 0)
    tree_sha=$(commit_tree_sha "$commit_sha")
    assert_matches_git_ls_tree "$tree_sha"
}

@test "qgit ls-tree <tree>: nested tree lists top level only" {
    setup_nested_tree_commit >/dev/null
    local commit_sha tree_sha
    commit_sha=$(commit_at 0)
    tree_sha=$(commit_tree_sha "$commit_sha")
    assert_matches_git_ls_tree "$tree_sha"
}

@test "qgit ls-tree <tree>: nested tree via commit matches git" {
    setup_nested_tree_commit >/dev/null
    local commit_sha
    commit_sha=$(commit_at 0)
    assert_matches_git_ls_tree "$commit_sha"
}

@test "qgit ls-tree <tree>: mixed modes match git" {
    setup_mixed_mode_tree_commit >/dev/null
    local commit_sha tree_sha
    commit_sha=$(commit_at 0)
    tree_sha=$(commit_tree_sha "$commit_sha")
    assert_matches_git_ls_tree "$tree_sha"
}

@test "qgit ls-tree <tree>: empty tree object matches git" {
    setup_empty_tree_commit >/dev/null
    assert_matches_git_ls_tree "$LS_TREE_EMPTY_TREE"
}

@test "qgit ls-tree <tree>: deep nested lists top level only" {
    setup_deep_nested_tree_commit >/dev/null
    local commit_sha tree_sha
    commit_sha=$(commit_at 0)
    tree_sha=$(commit_tree_sha "$commit_sha")
    assert_matches_git_ls_tree "$tree_sha"
    run_qgit_ls_tree "$tree_sha"
    assert_success
    assert_output_not_contains "sub/mid/leaf.txt"
}

@test "qgit ls-tree <tree>: inner tree sha matches git" {
    setup_nested_tree_commit >/dev/null
    local root_tree inner_tree
    root_tree=$(commit_tree_sha "$(commit_at 0)")
    inner_tree=$(nested_inner_tree_sha "$root_tree")
    assert_matches_git_ls_tree "$inner_tree"
}

# branch ref

@test "qgit ls-tree <branch>: default branch matches git" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_ls_tree "$LS_TREE_BRANCH"
}

@test "qgit ls-tree <branch>: non-default branch at older commit matches git" {
    setup_multi_branch_history
    assert_matches_git_ls_tree dev
}

@test "qgit ls-tree <branch>: branch name with slash matches git" {
    setup_multi_branch_history
    assert_matches_git_ls_tree release/v1
}

@test "qgit ls-tree <branch>: non-default branch matches its commit tip" {
    setup_multi_branch_history
    local dev_sha
    dev_sha=$(commit_at 1)
    assert_matches_git_ls_tree dev
    assert_matches_git_ls_tree "$dev_sha"
}

@test "qgit ls-tree <branch>: default branch differs from older branch" {
    setup_multi_branch_history
    local main_tree dev_tree
    main_tree=$(git_ls_tree "$LS_TREE_BRANCH")
    dev_tree=$(git_ls_tree dev)
    [ "$main_tree" != "$dev_tree" ] || return 1
}

@test "qgit ls-tree HEAD: follows non-default branch when HEAD points there" {
    setup_multi_branch_history
    set_head_to_branch dev
    assert_matches_git_ls_tree HEAD
}

@test "qgit ls-tree HEAD: matches explicit branch when HEAD points there" {
    setup_multi_branch_history
    set_head_to_branch release/v1
    local expected
    expected=$(git_ls_tree release/v1)
    run_qgit_ls_tree HEAD
    assert_success
    assert_output_equals "$expected"
}

# tree content

@test "qgit ls-tree: multi-entry tree matches git" {
    setup_multi_entry_tree_commit >/dev/null
    assert_matches_git_ls_tree HEAD
}

@test "qgit ls-tree: nested tree shows subdirectory entry" {
    setup_nested_tree_commit >/dev/null
    run_qgit_ls_tree HEAD
    assert_success
    assert_output_contains $'\tsubdir'
    assert_output_contains "040000 tree"
}

@test "qgit ls-tree: nested tree without recurse hides nested paths" {
    setup_nested_tree_commit >/dev/null
    assert_matches_git_ls_tree HEAD
    run_qgit_ls_tree HEAD
    assert_success
    assert_output_not_contains "subdir/inner.txt"
}

@test "qgit ls-tree: mixed mode tree matches git" {
    setup_mixed_mode_tree_commit >/dev/null
    assert_matches_git_ls_tree HEAD
}

@test "qgit ls-tree: empty tree commit produces no output" {
    setup_empty_tree_commit >/dev/null
    assert_matches_git_ls_tree HEAD
}

# -r

@test "qgit ls-tree -r: flat tree matches git" {
    setup_single_file_commit >/dev/null
    assert_matches_git_ls_tree -r
}

@test "qgit ls-tree -r: multi-entry tree matches git" {
    setup_multi_entry_tree_commit >/dev/null
    assert_matches_git_ls_tree -r
}

@test "qgit ls-tree -r: nested tree matches git" {
    setup_nested_tree_commit >/dev/null
    assert_matches_git_ls_tree -r
}

@test "qgit ls-tree -r: nested tree lists files under prefix" {
    setup_nested_tree_commit >/dev/null
    run_qgit_ls_tree -r HEAD
    assert_success
    assert_output_contains "subdir/inner.txt"
    assert_output_not_contains "040000 tree"
}

@test "qgit ls-tree -r: deep nested tree matches git" {
    setup_deep_nested_tree_commit >/dev/null
    assert_matches_git_ls_tree -r
}

@test "qgit ls-tree -r: deep nested tree lists nested paths" {
    setup_deep_nested_tree_commit >/dev/null
    run_qgit_ls_tree -r HEAD
    assert_success
    assert_output_contains "sub/mid/leaf.txt"
    assert_output_contains "top.txt"
}

@test "qgit ls-tree -r: mixed mode tree matches git" {
    setup_mixed_mode_tree_commit >/dev/null
    assert_matches_git_ls_tree -r
}

@test "qgit ls-tree -r: empty tree matches git" {
    setup_empty_tree_commit >/dev/null
    assert_matches_git_ls_tree -r
}

@test "qgit ls-tree -r HEAD: explicit HEAD matches git" {
    setup_nested_tree_commit >/dev/null
    assert_matches_git_ls_tree -r HEAD
}

@test "qgit ls-tree -r <commit>: commit tree matches git" {
    setup_nested_tree_commit >/dev/null
    local commit_sha
    commit_sha=$(commit_at 0)
    assert_matches_git_ls_tree -r "$commit_sha"
}

@test "qgit ls-tree -r <tree>: direct tree sha matches git" {
    setup_nested_tree_commit >/dev/null
    local commit_sha tree_sha
    commit_sha=$(commit_at 0)
    tree_sha=$(commit_tree_sha "$commit_sha")
    assert_matches_git_ls_tree -r "$tree_sha"
}

@test "qgit ls-tree -r <branch>: branch ref matches git" {
    setup_multi_branch_history
    assert_matches_git_ls_tree -r dev
}

@test "qgit ls-tree -r <commit>: merge commit matches git" {
    setup_merge_tip_commit >/dev/null
    local merge_tip
    merge_tip=$(commit_at 2)
    assert_matches_git_ls_tree -r "$merge_tip"
}

@test "qgit ls-tree -r <tree>: inner tree sha matches git" {
    setup_nested_tree_commit >/dev/null
    local root_tree inner_tree
    root_tree=$(commit_tree_sha "$(commit_at 0)")
    inner_tree=$(nested_inner_tree_sha "$root_tree")
    assert_matches_git_ls_tree -r "$inner_tree"
}

# -r -t

@test "qgit ls-tree -r -t: nested tree matches git" {
    setup_nested_tree_commit >/dev/null
    assert_matches_git_ls_tree -r -t
}

@test "qgit ls-tree -r -t: nested tree includes tree entries" {
    setup_nested_tree_commit >/dev/null
    run_qgit_ls_tree -r -t HEAD
    assert_success
    assert_output_contains "040000 tree"
    assert_output_contains $'\tsubdir'
    assert_output_contains "subdir/inner.txt"
}

@test "qgit ls-tree -r -t: deep nested tree matches git" {
    setup_deep_nested_tree_commit >/dev/null
    assert_matches_git_ls_tree -r -t
}

@test "qgit ls-tree -r -t: deep nested tree includes intermediate trees" {
    setup_deep_nested_tree_commit >/dev/null
    run_qgit_ls_tree -r -t HEAD
    assert_success
    assert_output_contains $'\tsub'
    assert_output_contains "sub/mid"
    assert_output_contains "sub/mid/leaf.txt"
}

@test "qgit ls-tree -r -t: mixed mode tree matches git" {
    setup_mixed_mode_tree_commit >/dev/null
    assert_matches_git_ls_tree -r -t
}

@test "qgit ls-tree -r -t: flat tree matches git" {
    setup_single_file_commit >/dev/null
    assert_matches_git_ls_tree -r -t
}

@test "qgit ls-tree -t -r: flag order matches git" {
    setup_nested_tree_commit >/dev/null
    assert_matches_git_ls_tree -t -r
}

@test "qgit ls-tree -r -t HEAD: explicit HEAD matches git" {
    setup_nested_tree_commit >/dev/null
    assert_matches_git_ls_tree -r -t HEAD
}

@test "qgit ls-tree -r -t <commit>: commit tree matches git" {
    setup_merge_tip_commit >/dev/null
    local merge_tip
    merge_tip=$(commit_at 2)
    assert_matches_git_ls_tree -r -t "$merge_tip"
}

@test "qgit ls-tree -r -t: empty tree matches git" {
    setup_empty_tree_commit >/dev/null
    assert_matches_git_ls_tree -r -t
}

@test "qgit ls-tree -r -t <tree>: direct tree sha matches git" {
    setup_nested_tree_commit >/dev/null
    local commit_sha tree_sha
    commit_sha=$(commit_at 0)
    tree_sha=$(commit_tree_sha "$commit_sha")
    assert_matches_git_ls_tree -r -t "$tree_sha"
}

@test "qgit ls-tree -r -t <tree>: inner tree sha matches git" {
    setup_nested_tree_commit >/dev/null
    local root_tree inner_tree
    root_tree=$(commit_tree_sha "$(commit_at 0)")
    inner_tree=$(nested_inner_tree_sha "$root_tree")
    assert_matches_git_ls_tree -r -t "$inner_tree"
}

@test "qgit ls-tree -r -t <branch>: branch with slash matches git" {
    setup_multi_branch_history
    assert_matches_git_ls_tree -r -t release/v1
}

# -t

@test "qgit ls-tree -t: without recurse matches git" {
    setup_nested_tree_commit >/dev/null
    assert_matches_git_ls_tree -t
}

@test "qgit ls-tree -t: without recurse shows top level only" {
    setup_nested_tree_commit >/dev/null
    run_qgit_ls_tree -t HEAD
    assert_success
    assert_output_contains $'\tsubdir'
    assert_output_not_contains "subdir/inner.txt"
}

@test "qgit ls-tree -t: without recurse matches plain output" {
    setup_nested_tree_commit >/dev/null
    local plain with_t
    plain=$(git_ls_tree HEAD)
    with_t=$(git_ls_tree -t HEAD)
    run_qgit_ls_tree -t HEAD
    assert_success
    assert_output_equals "$with_t"
    [ "$plain" = "$with_t" ] || return 1
}

# peel suffix

@test "qgit ls-tree: annotated tag name matches git" {
    setup_annotated_tag_on_tip v1.0 >/dev/null
    assert_matches_git_ls_tree v1.0
}

@test "qgit ls-tree: annotated tag^{tree} matches git" {
    setup_annotated_tag_on_tip v1.0 >/dev/null
    assert_matches_git_ls_tree "v1.0^{tree}"
}

@test "qgit ls-tree: annotated tag^{} matches git" {
    setup_annotated_tag_on_tip v1.0 >/dev/null
    assert_matches_git_ls_tree "v1.0^{}"
}

@test "qgit ls-tree -r: annotated tag name matches git" {
    setup_annotated_tag_on_tip v1.0 >/dev/null
    assert_matches_git_ls_tree -r v1.0
}

@test "qgit ls-tree -r: annotated tag^{tree} matches git" {
    setup_annotated_tag_on_tip v1.0 >/dev/null
    assert_matches_git_ls_tree -r "v1.0^{tree}"
}

@test "qgit ls-tree -r: annotated tag^{} matches git" {
    setup_annotated_tag_on_tip v1.0 >/dev/null
    assert_matches_git_ls_tree -r "v1.0^{}"
}

@test "qgit ls-tree -r -t: nested annotated tag matches git" {
    setup_annotated_tag_on_tip release/v1.0 "Nested release" >/dev/null
    assert_matches_git_ls_tree -r -t release/v1.0
}

@test "qgit ls-tree: annotated tag^{tag} matches git" {
    setup_annotated_tag_on_tip v1.0 >/dev/null
    assert_matches_git_ls_tree "v1.0^{tag}"
}

# errors

@test "qgit ls-tree: missing tree-ish fails" {
    setup_single_file_commit >/dev/null
    run_qgit_ls_tree
    assert_failure
}

@test "qgit ls-tree: outside repository fails" {
    mkdir outside && cd outside || return 1
    run_qgit_ls_tree HEAD
    assert_failure
}

@test "qgit ls-tree: unknown reference fails" {
    setup_single_file_commit >/dev/null
    run_qgit_ls_tree not-a-ref
    assert_failure
}

@test "qgit ls-tree: unknown branch name fails" {
    setup_multi_branch_history
    run_qgit_ls_tree no-such-branch
    assert_failure
}

@test "qgit ls-tree: unknown nested branch name fails" {
    setup_multi_branch_history
    run_qgit_ls_tree release/v2
    assert_failure
}

@test "qgit ls-tree: missing object fails" {
    setup_single_file_commit >/dev/null
    run_qgit_ls_tree 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit ls-tree: blob object fails" {
    setup_single_file_commit >/dev/null
    local blob_sha
    blob_sha=$(git_write_blob blob-single.txt)
    run_qgit_ls_tree "$blob_sha"
    assert_failure
}

@test "qgit ls-tree: invalid tree-ish on empty repo fails" {
    "$QGIT_BIN" init -q
    run_qgit_ls_tree HEAD
    assert_failure
}

@test "qgit ls-tree -r: missing tree-ish fails" {
    setup_single_file_commit >/dev/null
    run_qgit_ls_tree -r
    assert_failure
}

@test "qgit ls-tree -r: blob object fails" {
    setup_single_file_commit >/dev/null
    local blob_sha
    blob_sha=$(git_write_blob blob-single.txt)
    run_qgit_ls_tree -r "$blob_sha"
    assert_failure
}

@test "qgit ls-tree -r: missing object fails" {
    setup_single_file_commit >/dev/null
    run_qgit_ls_tree -r 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit ls-tree -r: unknown reference fails" {
    setup_single_file_commit >/dev/null
    run_qgit_ls_tree -r not-a-ref
    assert_failure
}

@test "qgit ls-tree -r -t: missing tree-ish fails" {
    setup_nested_tree_commit >/dev/null
    run_qgit_ls_tree -r -t
    assert_failure
}

@test "qgit ls-tree -r -t: blob object fails" {
    setup_nested_tree_commit >/dev/null
    local blob_sha
    blob_sha=$(git_write_blob blob-inner.txt)
    run_qgit_ls_tree -r -t "$blob_sha"
    assert_failure
}

@test "qgit ls-tree -r -t: missing object fails" {
    setup_nested_tree_commit >/dev/null
    run_qgit_ls_tree -r -t 0000000000000000000000000000000000000000
    assert_failure
}

# -h

@test "qgit ls-tree -h: shows help" {
    run_qgit_ls_tree -h
    assert_success
    assert_output_contains "qgit ls-tree"
    assert_output_contains "[-r]"
    assert_output_contains "-t"
    assert_output_contains "tree-ish"
}

@test "qgit ls-tree --help: shows help" {
    run_qgit_ls_tree --help
    assert_success
    assert_output_contains "qgit ls-tree"
    assert_output_contains "List the contents of a tree object"
    assert_output_contains "Recurse into sub-tree"
    assert_output_contains "Show tree entries even when going to recurse them"
}
