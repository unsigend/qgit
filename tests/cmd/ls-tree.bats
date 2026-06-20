load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

# helpers

LS_TREE_BRANCH="${QGIT_DEFAULT_BRANCH}"
LS_TREE_EMPTY_TREE="4b825dc642cb6eb9a060e54bf8d69288fbee4904"

ls_tree_chain_file() {
    printf '%s/ls_tree_chain.txt' "$TEST_DIR"
}

run_qgit_ls_tree() {
    run "$QGIT_BIN" ls-tree "$@"
}

git_ls_tree() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" ls-tree "$@"
}

assert_matches_git_ls_tree() {
    local expected qgit_args=("$@") git_args=("$@")
    local last="${@: -1}"

    # qgit and git require an explicit tree-ish; append HEAD when only flags are given.
    if [ $# -gt 0 ] && [[ "$last" == -* ]]; then
        qgit_args=("$@" HEAD)
        git_args=("$@" HEAD)
    fi
    expected=$(git_ls_tree "${git_args[@]}")
    run_qgit_ls_tree "${qgit_args[@]}"
    assert_success
    assert_output_equals "$expected"
}

init_ls_tree_repos() {
    "$QGIT_BIN" init -q
    env GIT_DIR="$TEST_DIR/.git" "$GIT" init -q
}

git_write_blob() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" hash-object -w "$@"
}

git_write_tree() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" mktree
}

git_write_commit() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" hash-object -w -t commit "$@"
}

write_tree_for_blob() {
    local blob_file="$1"
    local entry_name="$2"
    local blob_sha
    blob_sha=$(git_write_blob "$blob_file")
    printf '100644 blob %s\t%s\n' "$blob_sha" "$entry_name" | git_write_tree
}

copy_loose_objects() {
    local src="$TEST_DIR/.git/objects"
    local dst="$TEST_DIR/.qgit/objects"
    local dir base

    mkdir -p "$dst"
    for dir in "$src"/*/; do
        [ -d "$dir" ] || continue
        base=$(basename "$dir")
        case "$base" in
            pack|info) continue ;;
        esac
        mkdir -p "$dst/$base"
        cp -f "$dir"/* "$dst/$base/" 2>/dev/null || true
    done
}

set_branch_ref() {
    local sha="$1"
    local branch="${2:-$LS_TREE_BRANCH}"

    mkdir -p "$TEST_DIR/.qgit/refs/heads" "$TEST_DIR/.git/refs/heads"
    printf '%s\n' "$sha" > "$TEST_DIR/.qgit/refs/heads/$branch"
    printf '%s\n' "$sha" > "$TEST_DIR/.git/refs/heads/$branch"
    printf 'ref: refs/heads/%s\n' "$branch" > "$TEST_DIR/.qgit/HEAD"
    printf 'ref: refs/heads/%s\n' "$branch" > "$TEST_DIR/.git/HEAD"
}

set_extra_branch_ref() {
    local sha="$1"
    local branch="$2"
    local parent qgit_dir git_dir

    parent=$(dirname "$branch")
    qgit_dir="$TEST_DIR/.qgit/refs/heads"
    git_dir="$TEST_DIR/.git/refs/heads"

    if [ "$parent" != "." ]; then
        mkdir -p "$qgit_dir/$parent" "$git_dir/$parent"
    else
        mkdir -p "$qgit_dir" "$git_dir"
    fi
    printf '%s\n' "$sha" > "$qgit_dir/$branch"
    printf '%s\n' "$sha" > "$git_dir/$branch"
}

set_head_to_branch() {
    local branch="$1"

    printf 'ref: refs/heads/%s\n' "$branch" > "$TEST_DIR/.qgit/HEAD"
    printf 'ref: refs/heads/%s\n' "$branch" > "$TEST_DIR/.git/HEAD"
}

finalize_ls_tree_repo() {
    local tip_sha="$1"
    copy_loose_objects
    set_branch_ref "$tip_sha"
    echo "$tip_sha"
}

commit_at() {
    local idx="$1"
    sed -n "$((idx + 1))p" "$(ls_tree_chain_file)"
}

commit_tree_sha() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" rev-parse "$1^{tree}"
}

nested_inner_tree_sha() {
    local root_tree="$1"
    env GIT_DIR="$TEST_DIR/.git" "$GIT" rev-parse "$root_tree:subdir"
}

setup_single_file_commit() {
    local tree_sha commit_sha chain

    init_ls_tree_repos
    chain=$(ls_tree_chain_file)
    : > "$chain"

    printf 'hello\n' > blob-single.txt
    tree_sha=$(write_tree_for_blob blob-single.txt single.txt)
    cat > commit-single.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

single file tree
EOF
    commit_sha=$(git_write_commit commit-single.txt)
    printf '%s\n' "$commit_sha" >> "$chain"

    finalize_ls_tree_repo "$commit_sha"
}

setup_multi_entry_tree_commit() {
    local blob_a blob_b blob_c tree_sha commit_sha chain

    init_ls_tree_repos
    chain=$(ls_tree_chain_file)
    : > "$chain"

    printf 'alpha\n' > blob-alpha.txt
    printf 'beta\n' > blob-beta.txt
    printf 'gamma\n' > blob-gamma.txt
    blob_a=$(git_write_blob blob-alpha.txt)
    blob_b=$(git_write_blob blob-beta.txt)
    blob_c=$(git_write_blob blob-gamma.txt)
    tree_sha=$(
        {
            printf '100644 blob %s\talpha.txt\n' "$blob_a"
            printf '100644 blob %s\tbeta.txt\n' "$blob_b"
            printf '100644 blob %s\tgamma.txt\n' "$blob_c"
        } | git_write_tree
    )

    cat > commit-multi.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

multi entry tree
EOF
    commit_sha=$(git_write_commit commit-multi.txt)
    printf '%s\n' "$commit_sha" >> "$chain"

    finalize_ls_tree_repo "$commit_sha"
}

setup_nested_tree_commit() {
    local inner_blob inner_tree root_blob tree_sha commit_sha chain

    init_ls_tree_repos
    chain=$(ls_tree_chain_file)
    : > "$chain"

    printf 'inner\n' > blob-inner.txt
    inner_blob=$(git_write_blob blob-inner.txt)
    inner_tree=$(printf '100644 blob %s\tinner.txt\n' "$inner_blob" | git_write_tree)

    printf 'root\n' > blob-root.txt
    root_blob=$(git_write_blob blob-root.txt)
    tree_sha=$(
        {
            printf '100644 blob %s\troot.txt\n' "$root_blob"
            printf '040000 tree %s\tsubdir\n' "$inner_tree"
        } | git_write_tree
    )

    cat > commit-nested.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

nested tree
EOF
    commit_sha=$(git_write_commit commit-nested.txt)
    printf '%s\n' "$commit_sha" >> "$chain"

    finalize_ls_tree_repo "$commit_sha"
}

setup_mixed_mode_tree_commit() {
    local blob_reg blob_exec blob_link inner_tree tree_sha commit_sha chain

    init_ls_tree_repos
    chain=$(ls_tree_chain_file)
    : > "$chain"

    printf 'regular\n' > blob-regular.txt
    printf 'executable\n' > blob-executable.txt
    blob_reg=$(git_write_blob blob-regular.txt)
    blob_exec=$(git_write_blob blob-executable.txt)
    blob_link=$(git_write_blob blob-regular.txt)
    inner_tree=$(printf '100644 blob %s\tinner.txt\n' "$blob_reg" | git_write_tree)

    tree_sha=$(
        {
            printf '100644 blob %s\tregular.txt\n' "$blob_reg"
            printf '100755 blob %s\texecutable.sh\n' "$blob_exec"
            printf '120000 blob %s\tlink.txt\n' "$blob_link"
            printf '040000 tree %s\tdirectory\n' "$inner_tree"
        } | git_write_tree
    )

    cat > commit-mixed.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

mixed mode tree
EOF
    commit_sha=$(git_write_commit commit-mixed.txt)
    printf '%s\n' "$commit_sha" >> "$chain"

    finalize_ls_tree_repo "$commit_sha"
}

ensure_empty_tree_object() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" mktree < /dev/null >/dev/null
}

setup_empty_tree_commit() {
    local commit_sha chain

    init_ls_tree_repos
    chain=$(ls_tree_chain_file)
    : > "$chain"
    ensure_empty_tree_object

    cat > commit-empty-tree.txt <<EOF
tree $LS_TREE_EMPTY_TREE
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

empty tree commit
EOF
    commit_sha=$(git_write_commit commit-empty-tree.txt)
    printf '%s\n' "$commit_sha" >> "$chain"

    finalize_ls_tree_repo "$commit_sha"
}

setup_linear_history() {
    local count="$1"
    local i tree_sha parent_sha="" ts chain

    init_ls_tree_repos
    chain=$(ls_tree_chain_file)
    : > "$chain"

    for ((i = 0; i < count; i++)); do
        printf 'v%s\n' "$i" > "blob-v$i.txt"
        tree_sha=$(write_tree_for_blob "blob-v$i.txt" "v$i.txt")
        ts=$((946684800 + i * 86400))
        if [ -z "$parent_sha" ]; then
            cat > "commit-v$i.txt" <<EOF
tree $tree_sha
author Test User <test@example.com> $ts +0000
committer Test User <test@example.com> $ts +0000

version $i
EOF
        else
            cat > "commit-v$i.txt" <<EOF
tree $tree_sha
parent $parent_sha
author Test User <test@example.com> $ts +0000
committer Test User <test@example.com> $ts +0000

version $i
EOF
        fi
        parent_sha=$(git_write_commit "commit-v$i.txt")
        printf '%s\n' "$parent_sha" >> "$chain"
    done

    finalize_ls_tree_repo "$parent_sha"
}

setup_merge_tip_commit() {
    local root_sha commit_a_sha commit_b_sha merge_sha tree_sha chain

    init_ls_tree_repos
    chain=$(ls_tree_chain_file)
    : > "$chain"

    printf 'base\n' > blob-base.txt
    tree_sha=$(write_tree_for_blob blob-base.txt base.txt)
    cat > commit-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

root message
EOF
    root_sha=$(git_write_commit commit-root.txt)
    printf '%s\n' "$root_sha" >> "$chain"

    printf 'branch-a\n' > blob-a.txt
    tree_sha=$(write_tree_for_blob blob-a.txt a.txt)
    cat > commit-a.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0000
committer Test User <test@example.com> 946771200 +0000

first parent branch
EOF
    commit_a_sha=$(git_write_commit commit-a.txt)
    printf '%s\n' "$commit_a_sha" >> "$chain"

    printf 'branch-b\n' > blob-b.txt
    tree_sha=$(write_tree_for_blob blob-b.txt b.txt)
    cat > commit-b.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946857600 +0000
committer Test User <test@example.com> 946857600 +0000

second parent branch
EOF
    commit_b_sha=$(git_write_commit commit-b.txt)

    printf 'merged\n' > blob-merge.txt
    tree_sha=$(write_tree_for_blob blob-merge.txt merged.txt)
    cat > commit-merge.txt <<EOF
tree $tree_sha
parent $commit_a_sha
parent $commit_b_sha
author Test User <test@example.com> 946944000 +0000
committer Test User <test@example.com> 946944000 +0000

merge tip
EOF
    merge_sha=$(git_write_commit commit-merge.txt)
    printf '%s\n' "$merge_sha" >> "$chain"

    finalize_ls_tree_repo "$merge_sha"
}

setup_multi_branch_history() {
    local root_sha mid_sha

    setup_linear_history 4 >/dev/null
    root_sha=$(commit_at 0)
    mid_sha=$(commit_at 1)
    set_extra_branch_ref "$mid_sha" "dev"
    set_extra_branch_ref "$root_sha" "release/v1"
}

setup_deep_nested_tree_commit() {
    local leaf_blob leaf_tree mid_tree root_blob tree_sha commit_sha chain

    init_ls_tree_repos
    chain=$(ls_tree_chain_file)
    : > "$chain"

    printf 'leaf\n' > blob-leaf.txt
    leaf_blob=$(git_write_blob blob-leaf.txt)
    leaf_tree=$(printf '100644 blob %s\tleaf.txt\n' "$leaf_blob" | git_write_tree)

    mid_tree=$(printf '040000 tree %s\tmid\n' "$leaf_tree" | git_write_tree)

    printf 'top\n' > blob-top.txt
    root_blob=$(git_write_blob blob-top.txt)
    tree_sha=$(
        {
            printf '100644 blob %s\ttop.txt\n' "$root_blob"
            printf '040000 tree %s\tsub\n' "$mid_tree"
        } | git_write_tree
    )

    cat > commit-deep-nested.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

deep nested tree
EOF
    commit_sha=$(git_write_commit commit-deep-nested.txt)
    printf '%s\n' "$commit_sha" >> "$chain"

    finalize_ls_tree_repo "$commit_sha"
}

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
