load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"
load "helpers/commit_messages.bash"

# helpers

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
    printf '%s\n' "$sha" > "$TEST_DIR/.qgit/refs/heads/$branch"
    printf '%s\n' "$sha" > "$TEST_DIR/.git/refs/heads/$branch"
    printf 'ref: refs/heads/%s\n' "$branch" > "$TEST_DIR/.qgit/HEAD"
    printf 'ref: refs/heads/%s\n' "$branch" > "$TEST_DIR/.git/HEAD"
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

setup_linear_history() {
    local count="$1"
    local i tree_sha parent_sha="" ts chain

    init_log_repos
    chain=$(log_chain_file)
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

    finalize_log_history "$parent_sha"
}

setup_merge_first_parent_history() {
    local root_sha commit_a_sha commit_b_sha merge_sha tree_sha chain

    init_log_repos
    chain=$(log_chain_file)
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

    finalize_log_history "$merge_sha"
}

setup_utf8_history() {
    local root_sha child_sha tree_sha chain

    init_log_repos
    chain=$(log_chain_file)
    : > "$chain"

    printf 'utf8\n' > blob-root.txt
    tree_sha=$(write_tree_for_blob blob-root.txt root.txt)
    cat > commit-root.txt <<EOF
tree $tree_sha
author O'Brien <obrien@example.com> 946684800 +0000
committer O'Brien <obrien@example.com> 946684800 +0000

café root
EOF
    root_sha=$(git_write_commit commit-root.txt)
    printf '%s\n' "$root_sha" >> "$chain"

    printf 'utf8-child\n' > blob-child.txt
    tree_sha=$(write_tree_for_blob blob-child.txt child.txt)
    cat > commit-child.txt <<EOF
tree $tree_sha
parent $root_sha
author O'Brien <obrien@example.com> 946771200 +0000
committer O'Brien <obrien@example.com> 946771200 +0000

résumé child
EOF
    child_sha=$(git_write_commit commit-child.txt)
    printf '%s\n' "$child_sha" >> "$chain"

    finalize_log_history "$child_sha"
}

setup_timezone_history() {
    local root_sha child_sha tree_sha chain

    init_log_repos
    chain=$(log_chain_file)
    : > "$chain"

    printf 'tz-root\n' > blob-tz-root.txt
    tree_sha=$(write_tree_for_blob blob-tz-root.txt tz-root.txt)
    cat > commit-tz-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 -0700
committer Test User <test@example.com> 946684800 -0700

timezone root
EOF
    root_sha=$(git_write_commit commit-tz-root.txt)
    printf '%s\n' "$root_sha" >> "$chain"

    printf 'tz-child\n' > blob-tz-child.txt
    tree_sha=$(write_tree_for_blob blob-tz-child.txt tz-child.txt)
    cat > commit-tz-child.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0530
committer Test User <test@example.com> 946857600 +0845

timezone child
EOF
    child_sha=$(git_write_commit commit-tz-child.txt)
    printf '%s\n' "$child_sha" >> "$chain"

    finalize_log_history "$child_sha"
}

setup_different_author_committer_history() {
    local root_sha child_sha tree_sha chain

    init_log_repos
    chain=$(log_chain_file)
    : > "$chain"

    printf 'authored\n' > blob-author.txt
    tree_sha=$(write_tree_for_blob blob-author.txt author.txt)
    cat > commit-author-root.txt <<EOF
tree $tree_sha
author Alice Author <alice@example.com> 946684800 +0000
committer Alice Author <alice@example.com> 946684800 +0000

author root
EOF
    root_sha=$(git_write_commit commit-author-root.txt)
    printf '%s\n' "$root_sha" >> "$chain"

    printf 'committed\n' > blob-committer.txt
    tree_sha=$(write_tree_for_blob blob-committer.txt committer.txt)
    cat > commit-author-child.txt <<EOF
tree $tree_sha
parent $root_sha
author Alice Author <alice@example.com> 946771200 -0500
committer Bob Committer <bob@example.com> 946857600 +0900

different author and committer
EOF
    child_sha=$(git_write_commit commit-author-child.txt)
    printf '%s\n' "$child_sha" >> "$chain"

    finalize_log_history "$child_sha"
}

setup_empty_message_log() {
    local tree_sha commit_sha

    init_log_repos
    printf 'empty\n' > blob-empty.txt
    tree_sha=$(write_tree_for_blob blob-empty.txt empty.txt)
    write_root_commit_file commit-empty.txt "$tree_sha" ""
    commit_sha=$(git_write_commit commit-empty.txt)
    finalize_log_history "$commit_sha"
}

setup_multiline_log_history() {
    local tree_sha commit_sha

    init_log_repos
    printf 'multi\n' > blob-multi.txt
    tree_sha=$(write_tree_for_blob blob-multi.txt multi.txt)
    write_multiline_commit_file commit-multi.txt "$tree_sha"
    commit_sha=$(git_write_commit commit-multi.txt)
    finalize_log_history "$commit_sha"
}

setup_message_log() {
    local body="$1"
    local tree_sha commit_sha

    init_log_repos
    printf 'msg\n' > blob-msg.txt
    tree_sha=$(write_tree_for_blob blob-msg.txt msg.txt)
    write_root_commit_file commit-msg.txt "$tree_sha" "$body"
    commit_sha=$(git_write_commit commit-msg.txt)
    finalize_log_history "$commit_sha"
}

# log --first-parent

@test "qgit log --first-parent: linear history matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: single commit matches git" {
    setup_linear_history 1 >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: merge follows first parent only" {
    setup_merge_first_parent_history >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: merge skips second parent branch" {
    setup_merge_first_parent_history >/dev/null
    run_qgit_log --first-parent
    assert_success
    assert_output_not_contains "second parent branch"
}

# --oneline

@test "qgit log --oneline --first-parent: linear history matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log --oneline --first-parent
}

@test "qgit log --oneline --first-parent: single commit matches git" {
    setup_linear_history 1 >/dev/null
    assert_matches_git_log --oneline --first-parent
}

@test "qgit log --oneline --first-parent: merge matches git" {
    setup_merge_first_parent_history >/dev/null
    assert_matches_git_log --oneline --first-parent
}

# -n

@test "qgit log -n 1 --first-parent: limits to one commit" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log -n 1 --first-parent
}

@test "qgit log -n 2 --first-parent: limits to two commits" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log -n 2 --first-parent
}

@test "qgit log -n 3 --first-parent: limits to three commits" {
    setup_linear_history 5 >/dev/null
    assert_matches_git_log -n 3 --first-parent
}

@test "qgit log -n 0 --first-parent: shows no commits" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_log -n 0 --first-parent
}

@test "qgit log -n 10 --first-parent: shows all when limit exceeds history" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_log -n 10 --first-parent
}

@test "qgit log -n 1 --oneline --first-parent: one line limit matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log -n 1 --oneline --first-parent
}

@test "qgit log -n 2 --oneline --first-parent: one line limit matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log -n 2 --oneline --first-parent
}

# <commit>

@test "qgit log --first-parent HEAD: explicit HEAD matches git" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_log --first-parent HEAD
}

@test "qgit log --first-parent <commit>: starts from middle commit" {
    setup_linear_history 5 >/dev/null
    local mid
    mid=$(commit_at 2)
    assert_matches_git_log --first-parent "$mid"
}

@test "qgit log --first-parent <commit>: root commit shows one entry" {
    setup_linear_history 4 >/dev/null
    local root
    root=$(commit_at 0)
    assert_matches_git_log --first-parent "$root"
}

@test "qgit log --oneline --first-parent <commit>: middle commit matches git" {
    setup_linear_history 5 >/dev/null
    local mid
    mid=$(commit_at 2)
    assert_matches_git_log --oneline --first-parent "$mid"
}

@test "qgit log -n 1 --first-parent <commit>: limits from middle commit" {
    setup_linear_history 5 >/dev/null
    local mid
    mid=$(commit_at 3)
    assert_matches_git_log -n 1 --first-parent "$mid"
}

# edge cases

@test "qgit log --first-parent: utf8 messages match git" {
    setup_utf8_history >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: long linear chain matches git" {
    setup_linear_history 8 >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log -n 1 --first-parent: merge tip limits to one" {
    setup_merge_first_parent_history >/dev/null
    assert_matches_git_log -n 1 --first-parent
}

# message edge cases

@test "qgit log --first-parent: empty commit message matches git" {
    setup_empty_message_log >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: multiline message matches git" {
    setup_multiline_log_history >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --oneline --first-parent: multiline message matches git" {
    setup_multiline_log_history >/dev/null
    assert_matches_git_log --oneline --first-parent
}

@test "qgit log --first-parent: message starting with author matches git" {
    setup_message_log "author note only" >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: message starting with committer matches git" {
    setup_message_log "committer note only" >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: message starting with tree matches git" {
    setup_message_log "tree rebuild notes" >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: message starting with parent matches git" {
    setup_message_log "parent linkage notes" >/dev/null
    assert_matches_git_log --first-parent
}

# time and timezone

@test "qgit log --first-parent: timezone offsets match git" {
    setup_timezone_history >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --oneline --first-parent: timezone offsets match git" {
    setup_timezone_history >/dev/null
    assert_matches_git_log --oneline --first-parent
}

@test "qgit log --first-parent: different author and committer times match git" {
    setup_different_author_committer_history >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --oneline --first-parent: different author and committer times match git" {
    setup_different_author_committer_history >/dev/null
    assert_matches_git_log --oneline --first-parent
}

# errors

@test "qgit log --first-parent: outside repository fails" {
    mkdir outside && cd outside || return 1
    run_qgit_log --first-parent
    assert_failure
}

@test "qgit log --first-parent: unknown reference fails" {
    setup_linear_history 2 >/dev/null
    run_qgit_log --first-parent not-a-ref
    assert_failure
}

@test "qgit log --first-parent: missing object fails" {
    setup_linear_history 2 >/dev/null
    run_qgit_log --first-parent 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit log --first-parent: non-commit object fails" {
    setup_linear_history 2 >/dev/null
    local blob_sha
    blob_sha=$(git_write_blob blob-v0.txt)
    run_qgit_log --first-parent "$blob_sha"
    assert_failure
}

@test "qgit log -n: missing count fails" {
    setup_linear_history 2 >/dev/null
    run_qgit_log -n --first-parent
    assert_failure
}

# -h

@test "qgit log -h: shows help" {
    run_qgit_log -h
    assert_success
    assert_output_contains "qgit log"
    assert_output_contains "--first-parent"
}

@test "qgit log --help: shows help" {
    run_qgit_log --help
    assert_success
    assert_output_contains "qgit log"
    assert_output_contains "--oneline"
}
