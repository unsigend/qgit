LOG_BRANCH="${QGIT_DEFAULT_BRANCH}"

log_chain_file() {
    printf '%s/commit_chain.txt' "$TEST_DIR"
}

run_qgit_log() {
    run "$QGIT_BIN" log "$@"
}

git_log() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" log "$@"
}

assert_matches_git_log() {
    local expected
    expected=$(git_log "$@")
    run_qgit_log "$@"
    assert_success
    assert_output_matches_normalized "$expected"
}

init_log_repos() {
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
    local branch="${2:-$LOG_BRANCH}"

    mkdir -p "$TEST_DIR/.qgit/refs/heads" "$TEST_DIR/.git/refs/heads"
    printf '%s\n' "$sha" >"$TEST_DIR/.qgit/refs/heads/$branch"
    printf '%s\n' "$sha" >"$TEST_DIR/.git/refs/heads/$branch"
    printf 'ref: refs/heads/%s\n' "$branch" >"$TEST_DIR/.qgit/HEAD"
    printf 'ref: refs/heads/%s\n' "$branch" >"$TEST_DIR/.git/HEAD"
}

finalize_log_history() {
    local tip_sha="$1"

    copy_loose_objects
    set_branch_ref "$tip_sha"
    echo "$tip_sha"
}

commit_at() {
    local idx="$1"

    sed -n "$((idx + 1))p" "$(log_chain_file)"
}

abbrev_sha() {
    local sha="$1"
    local len="$2"

    printf '%s' "${sha:0:$len}"
}

setup_linear_history() {
    local count="$1"
    local i tree_sha parent_sha="" ts chain

    init_log_repos
    chain=$(log_chain_file)
    : >"$chain"

    for ((i = 0; i < count; i++)); do
        printf 'v%s\n' "$i" >"blob-v$i.txt"
        tree_sha=$(write_tree_for_blob "blob-v$i.txt" "v$i.txt")
        ts=$((946684800 + i * 86400))
        if [ -z "$parent_sha" ]; then
            cat >"commit-v$i.txt" <<EOF
tree $tree_sha
author Test User <test@example.com> $ts +0000
committer Test User <test@example.com> $ts +0000

version $i
EOF
        else
            cat >"commit-v$i.txt" <<EOF
tree $tree_sha
parent $parent_sha
author Test User <test@example.com> $ts +0000
committer Test User <test@example.com> $ts +0000

version $i
EOF
        fi
        parent_sha=$(git_write_commit "commit-v$i.txt")
        printf '%s\n' "$parent_sha" >>"$chain"
    done

    finalize_log_history "$parent_sha"
}

setup_merge_first_parent_history() {
    local root_sha commit_a_sha commit_b_sha merge_sha tree_sha chain

    init_log_repos
    chain=$(log_chain_file)
    : >"$chain"

    printf 'base\n' >blob-base.txt
    tree_sha=$(write_tree_for_blob blob-base.txt base.txt)
    cat >commit-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

root message
EOF
    root_sha=$(git_write_commit commit-root.txt)
    printf '%s\n' "$root_sha" >>"$chain"

    printf 'branch-a\n' >blob-a.txt
    tree_sha=$(write_tree_for_blob blob-a.txt a.txt)
    cat >commit-a.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0000
committer Test User <test@example.com> 946771200 +0000

first parent branch
EOF
    commit_a_sha=$(git_write_commit commit-a.txt)
    printf '%s\n' "$commit_a_sha" >>"$chain"

    printf 'branch-b\n' >blob-b.txt
    tree_sha=$(write_tree_for_blob blob-b.txt b.txt)
    cat >commit-b.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946857600 +0000
committer Test User <test@example.com> 946857600 +0000

second parent branch
EOF
    commit_b_sha=$(git_write_commit commit-b.txt)

    printf 'merged\n' >blob-merge.txt
    tree_sha=$(write_tree_for_blob blob-merge.txt merged.txt)
    cat >commit-merge.txt <<EOF
tree $tree_sha
parent $commit_a_sha
parent $commit_b_sha
author Test User <test@example.com> 946944000 +0000
committer Test User <test@example.com> 946944000 +0000

merge tip
EOF
    merge_sha=$(git_write_commit commit-merge.txt)
    printf '%s\n' "$merge_sha" >>"$chain"

    finalize_log_history "$merge_sha"
}

setup_diamond_merge_history() {
    local root_sha left_sha left_tip_sha right_sha right_tip_sha merge_sha tree_sha chain

    init_log_repos
    chain=$(log_chain_file)
    : >"$chain"

    printf 'diamond-root\n' >blob-diamond-root.txt
    tree_sha=$(write_tree_for_blob blob-diamond-root.txt diamond-root.txt)
    cat >commit-diamond-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

diamond root
EOF
    root_sha=$(git_write_commit commit-diamond-root.txt)
    printf '%s\n' "$root_sha" >>"$chain"

    printf 'diamond-left\n' >blob-diamond-left.txt
    tree_sha=$(write_tree_for_blob blob-diamond-left.txt diamond-left.txt)
    cat >commit-diamond-left.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0000
committer Test User <test@example.com> 946771200 +0000

diamond left branch
EOF
    left_sha=$(git_write_commit commit-diamond-left.txt)
    printf '%s\n' "$left_sha" >>"$chain"

    printf 'diamond-left-tip\n' >blob-diamond-left-tip.txt
    tree_sha=$(write_tree_for_blob blob-diamond-left-tip.txt diamond-left-tip.txt)
    cat >commit-diamond-left-tip.txt <<EOF
tree $tree_sha
parent $left_sha
author Test User <test@example.com> 946857600 +0000
committer Test User <test@example.com> 946857600 +0000

diamond left tip
EOF
    left_tip_sha=$(git_write_commit commit-diamond-left-tip.txt)
    printf '%s\n' "$left_tip_sha" >>"$chain"

    printf 'diamond-right\n' >blob-diamond-right.txt
    tree_sha=$(write_tree_for_blob blob-diamond-right.txt diamond-right.txt)
    cat >commit-diamond-right.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946728000 +0000
committer Test User <test@example.com> 946728000 +0000

diamond right branch
EOF
    right_sha=$(git_write_commit commit-diamond-right.txt)

    printf 'diamond-right-tip\n' >blob-diamond-right-tip.txt
    tree_sha=$(write_tree_for_blob blob-diamond-right-tip.txt diamond-right-tip.txt)
    cat >commit-diamond-right-tip.txt <<EOF
tree $tree_sha
parent $right_sha
author Test User <test@example.com> 946944000 +0000
committer Test User <test@example.com> 946944000 +0000

diamond right tip
EOF
    right_tip_sha=$(git_write_commit commit-diamond-right-tip.txt)

    printf 'diamond-merged\n' >blob-diamond-merge.txt
    tree_sha=$(write_tree_for_blob blob-diamond-merge.txt diamond-merge.txt)
    cat >commit-diamond-merge.txt <<EOF
tree $tree_sha
parent $left_tip_sha
parent $right_tip_sha
author Test User <test@example.com> 947030400 +0000
committer Test User <test@example.com> 947030400 +0000

diamond merge tip
EOF
    merge_sha=$(git_write_commit commit-diamond-merge.txt)
    printf '%s\n' "$merge_sha" >>"$chain"

    finalize_log_history "$merge_sha"
}

setup_octopus_merge_history() {
    local root_sha sha_a sha_b sha_c merge_sha tree_sha chain

    init_log_repos
    chain=$(log_chain_file)
    : >"$chain"

    printf 'octopus-root\n' >blob-octopus-root.txt
    tree_sha=$(write_tree_for_blob blob-octopus-root.txt octopus-root.txt)
    cat >commit-octopus-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

octopus root
EOF
    root_sha=$(git_write_commit commit-octopus-root.txt)
    printf '%s\n' "$root_sha" >>"$chain"

    printf 'octopus-a\n' >blob-octopus-a.txt
    tree_sha=$(write_tree_for_blob blob-octopus-a.txt octopus-a.txt)
    cat >commit-octopus-a.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0000
committer Test User <test@example.com> 946771200 +0000

octopus branch a
EOF
    sha_a=$(git_write_commit commit-octopus-a.txt)

    printf 'octopus-b\n' >blob-octopus-b.txt
    tree_sha=$(write_tree_for_blob blob-octopus-b.txt octopus-b.txt)
    cat >commit-octopus-b.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946857600 +0000
committer Test User <test@example.com> 946857600 +0000

octopus branch b
EOF
    sha_b=$(git_write_commit commit-octopus-b.txt)

    printf 'octopus-c\n' >blob-octopus-c.txt
    tree_sha=$(write_tree_for_blob blob-octopus-c.txt octopus-c.txt)
    cat >commit-octopus-c.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946944000 +0000
committer Test User <test@example.com> 946944000 +0000

octopus branch c
EOF
    sha_c=$(git_write_commit commit-octopus-c.txt)

    printf 'octopus-merged\n' >blob-octopus-merge.txt
    tree_sha=$(write_tree_for_blob blob-octopus-merge.txt octopus-merge.txt)
    cat >commit-octopus-merge.txt <<EOF
tree $tree_sha
parent $sha_a
parent $sha_b
parent $sha_c
author Test User <test@example.com> 947030400 +0000
committer Test User <test@example.com> 947030400 +0000

octopus merge tip
EOF
    merge_sha=$(git_write_commit commit-octopus-merge.txt)
    printf '%s\n' "$merge_sha" >>"$chain"

    finalize_log_history "$merge_sha"
}

setup_nested_merge_history() {
    local root_sha commit_a_sha commit_b_sha merge1_sha commit_c_sha commit_d_sha merge2_sha tree_sha chain

    init_log_repos
    chain=$(log_chain_file)
    : >"$chain"

    printf 'nested-root\n' >blob-nested-root.txt
    tree_sha=$(write_tree_for_blob blob-nested-root.txt nested-root.txt)
    cat >commit-nested-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

nested root
EOF
    root_sha=$(git_write_commit commit-nested-root.txt)
    printf '%s\n' "$root_sha" >>"$chain"

    printf 'nested-a\n' >blob-nested-a.txt
    tree_sha=$(write_tree_for_blob blob-nested-a.txt nested-a.txt)
    cat >commit-nested-a.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0000
committer Test User <test@example.com> 946771200 +0000

nested branch a
EOF
    commit_a_sha=$(git_write_commit commit-nested-a.txt)
    printf '%s\n' "$commit_a_sha" >>"$chain"

    printf 'nested-b\n' >blob-nested-b.txt
    tree_sha=$(write_tree_for_blob blob-nested-b.txt nested-b.txt)
    cat >commit-nested-b.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946857600 +0000
committer Test User <test@example.com> 946857600 +0000

nested branch b
EOF
    commit_b_sha=$(git_write_commit commit-nested-b.txt)

    printf 'nested-merge1\n' >blob-nested-merge1.txt
    tree_sha=$(write_tree_for_blob blob-nested-merge1.txt nested-merge1.txt)
    cat >commit-nested-merge1.txt <<EOF
tree $tree_sha
parent $commit_a_sha
parent $commit_b_sha
author Test User <test@example.com> 946944000 +0000
committer Test User <test@example.com> 946944000 +0000

nested first merge
EOF
    merge1_sha=$(git_write_commit commit-nested-merge1.txt)
    printf '%s\n' "$merge1_sha" >>"$chain"

    printf 'nested-c\n' >blob-nested-c.txt
    tree_sha=$(write_tree_for_blob blob-nested-c.txt nested-c.txt)
    cat >commit-nested-c.txt <<EOF
tree $tree_sha
parent $merge1_sha
author Test User <test@example.com> 947030400 +0000
committer Test User <test@example.com> 947030400 +0000

nested after first merge
EOF
    commit_c_sha=$(git_write_commit commit-nested-c.txt)
    printf '%s\n' "$commit_c_sha" >>"$chain"

    printf 'nested-d\n' >blob-nested-d.txt
    tree_sha=$(write_tree_for_blob blob-nested-d.txt nested-d.txt)
    cat >commit-nested-d.txt <<EOF
tree $tree_sha
parent $commit_b_sha
author Test User <test@example.com> 947116800 +0000
committer Test User <test@example.com> 947116800 +0000

nested continued b branch
EOF
    commit_d_sha=$(git_write_commit commit-nested-d.txt)

    printf 'nested-merge2\n' >blob-nested-merge2.txt
    tree_sha=$(write_tree_for_blob blob-nested-merge2.txt nested-merge2.txt)
    cat >commit-nested-merge2.txt <<EOF
tree $tree_sha
parent $commit_c_sha
parent $commit_d_sha
author Test User <test@example.com> 947203200 +0000
committer Test User <test@example.com> 947203200 +0000

nested second merge
EOF
    merge2_sha=$(git_write_commit commit-nested-merge2.txt)
    printf '%s\n' "$merge2_sha" >>"$chain"

    finalize_log_history "$merge2_sha"
}

setup_timezone_history() {
    local root_sha child_sha tree_sha chain

    init_log_repos
    chain=$(log_chain_file)
    : >"$chain"

    printf 'tz-root\n' >blob-tz-root.txt
    tree_sha=$(write_tree_for_blob blob-tz-root.txt tz-root.txt)
    cat >commit-tz-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 -0700
committer Test User <test@example.com> 946684800 -0700

timezone root
EOF
    root_sha=$(git_write_commit commit-tz-root.txt)
    printf '%s\n' "$root_sha" >>"$chain"

    printf 'tz-child\n' >blob-tz-child.txt
    tree_sha=$(write_tree_for_blob blob-tz-child.txt tz-child.txt)
    cat >commit-tz-child.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0530
committer Test User <test@example.com> 946857600 +0845

timezone child
EOF
    child_sha=$(git_write_commit commit-tz-child.txt)
    printf '%s\n' "$child_sha" >>"$chain"

    finalize_log_history "$child_sha"
}

setup_different_author_committer_history() {
    local root_sha child_sha tree_sha chain

    init_log_repos
    chain=$(log_chain_file)
    : >"$chain"

    printf 'authored\n' >blob-author.txt
    tree_sha=$(write_tree_for_blob blob-author.txt author.txt)
    cat >commit-author-root.txt <<EOF
tree $tree_sha
author Alice Author <alice@example.com> 946684800 +0000
committer Alice Author <alice@example.com> 946684800 +0000

author root
EOF
    root_sha=$(git_write_commit commit-author-root.txt)
    printf '%s\n' "$root_sha" >>"$chain"

    printf 'committed\n' >blob-committer.txt
    tree_sha=$(write_tree_for_blob blob-committer.txt committer.txt)
    cat >commit-author-child.txt <<EOF
tree $tree_sha
parent $root_sha
author Alice Author <alice@example.com> 946771200 -0500
committer Bob Committer <bob@example.com> 946857600 +0900

different author and committer
EOF
    child_sha=$(git_write_commit commit-author-child.txt)
    printf '%s\n' "$child_sha" >>"$chain"

    finalize_log_history "$child_sha"
}

setup_committer_order_merge_history() {
    local root_sha commit_alpha_sha commit_beta_sha merge_sha tree_sha chain

    init_log_repos
    chain=$(log_chain_file)
    : >"$chain"

    printf 'order-root\n' >blob-order-root.txt
    tree_sha=$(write_tree_for_blob blob-order-root.txt order-root.txt)
    cat >commit-order-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

order root
EOF
    root_sha=$(git_write_commit commit-order-root.txt)
    printf '%s\n' "$root_sha" >>"$chain"

    printf 'order-alpha\n' >blob-order-alpha.txt
    tree_sha=$(write_tree_for_blob blob-order-alpha.txt order-alpha.txt)
    cat >commit-order-alpha.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 947116800 +0000
committer Test User <test@example.com> 946771200 +0000

branch alpha
EOF
    commit_alpha_sha=$(git_write_commit commit-order-alpha.txt)

    printf 'order-beta\n' >blob-order-beta.txt
    tree_sha=$(write_tree_for_blob blob-order-beta.txt order-beta.txt)
    cat >commit-order-beta.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0000
committer Test User <test@example.com> 947116800 +0000

branch beta
EOF
    commit_beta_sha=$(git_write_commit commit-order-beta.txt)

    printf 'order-merge\n' >blob-order-merge.txt
    tree_sha=$(write_tree_for_blob blob-order-merge.txt order-merge.txt)
    cat >commit-order-merge.txt <<EOF
tree $tree_sha
parent $commit_alpha_sha
parent $commit_beta_sha
author Test User <test@example.com> 947203200 +0000
committer Test User <test@example.com> 947203200 +0000

order merge tip
EOF
    merge_sha=$(git_write_commit commit-order-merge.txt)
    printf '%s\n' "$merge_sha" >>"$chain"

    finalize_log_history "$merge_sha"
}

setup_committer_order_octopus_history() {
    local root_sha sha_a sha_b sha_c merge_sha tree_sha chain

    init_log_repos
    chain=$(log_chain_file)
    : >"$chain"

    printf 'oct-order-root\n' >blob-oct-order-root.txt
    tree_sha=$(write_tree_for_blob blob-oct-order-root.txt oct-order-root.txt)
    cat >commit-oct-order-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

octopus order root
EOF
    root_sha=$(git_write_commit commit-oct-order-root.txt)
    printf '%s\n' "$root_sha" >>"$chain"

    printf 'oct-order-a\n' >blob-oct-order-a.txt
    tree_sha=$(write_tree_for_blob blob-oct-order-a.txt oct-order-a.txt)
    cat >commit-oct-order-a.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 947203200 +0000
committer Test User <test@example.com> 946771200 +0000

octopus order a
EOF
    sha_a=$(git_write_commit commit-oct-order-a.txt)

    printf 'oct-order-b\n' >blob-oct-order-b.txt
    tree_sha=$(write_tree_for_blob blob-oct-order-b.txt oct-order-b.txt)
    cat >commit-oct-order-b.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0000
committer Test User <test@example.com> 947203200 +0000

octopus order b
EOF
    sha_b=$(git_write_commit commit-oct-order-b.txt)

    printf 'oct-order-c\n' >blob-oct-order-c.txt
    tree_sha=$(write_tree_for_blob blob-oct-order-c.txt oct-order-c.txt)
    cat >commit-oct-order-c.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 947116800 +0000
committer Test User <test@example.com> 947116800 +0000

octopus order c
EOF
    sha_c=$(git_write_commit commit-oct-order-c.txt)

    printf 'oct-order-merge\n' >blob-oct-order-merge.txt
    tree_sha=$(write_tree_for_blob blob-oct-order-merge.txt oct-order-merge.txt)
    cat >commit-oct-order-merge.txt <<EOF
tree $tree_sha
parent $sha_a
parent $sha_b
parent $sha_c
author Test User <test@example.com> 947289600 +0000
committer Test User <test@example.com> 947289600 +0000

octopus order merge
EOF
    merge_sha=$(git_write_commit commit-oct-order-merge.txt)
    printf '%s\n' "$merge_sha" >>"$chain"

    finalize_log_history "$merge_sha"
}

setup_empty_message_log() {
    local tree_sha commit_sha

    init_log_repos
    printf 'empty\n' >blob-empty.txt
    tree_sha=$(write_tree_for_blob blob-empty.txt empty.txt)
    write_root_commit_file commit-empty.txt "$tree_sha" ""
    commit_sha=$(git_write_commit commit-empty.txt)
    finalize_log_history "$commit_sha"
}

setup_multiline_log_history() {
    local tree_sha commit_sha

    init_log_repos
    printf 'multi\n' >blob-multi.txt
    tree_sha=$(write_tree_for_blob blob-multi.txt multi.txt)
    write_multiline_commit_file commit-multi.txt "$tree_sha"
    commit_sha=$(git_write_commit commit-multi.txt)
    finalize_log_history "$commit_sha"
}

setup_message_log() {
    local body="$1"
    local tree_sha commit_sha

    init_log_repos
    printf 'msg\n' >blob-msg.txt
    tree_sha=$(write_tree_for_blob blob-msg.txt msg.txt)
    write_root_commit_file commit-msg.txt "$tree_sha" "$body"
    commit_sha=$(git_write_commit commit-msg.txt)
    finalize_log_history "$commit_sha"
}
