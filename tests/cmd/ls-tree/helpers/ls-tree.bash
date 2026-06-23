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

setup_git_tag_identity() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" config user.name "Test User"
    env GIT_DIR="$TEST_DIR/.git" "$GIT" config user.email "test@example.com"
}

write_tag_ref() {
    local tagname="$1"
    local sha="$2"
    local dir

    dir=$(dirname "$tagname")
    mkdir -p "$TEST_DIR/.git/refs/tags/$dir" "$TEST_DIR/.qgit/refs/tags/$dir"
    printf '%s\n' "$sha" >"$TEST_DIR/.git/refs/tags/$tagname"
    printf '%s\n' "$sha" >"$TEST_DIR/.qgit/refs/tags/$tagname"
}

setup_annotated_tag_on_tip() {
    local tagname="${1:-v1.0}"
    local message="${2:-Release tag}"
    local tip_sha tag_obj_sha

    tip_sha=$(setup_nested_tree_commit)
    setup_git_tag_identity
    env GIT_DIR="$TEST_DIR/.git" "$GIT" tag -a "$tagname" -m "$message" "$tip_sha"
    copy_loose_objects
    tag_obj_sha=$(env GIT_DIR="$TEST_DIR/.git" "$GIT" rev-parse "refs/tags/$tagname")
    write_tag_ref "$tagname" "$tag_obj_sha"
    LS_TREE_PEEL_TAG="$tagname"
    LS_TREE_PEEL_TIP="$tip_sha"
    printf '%s\n' "$tip_sha"
}
