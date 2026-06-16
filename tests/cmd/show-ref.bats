load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

# helpers

SHOW_REF_BRANCH="${QGIT_DEFAULT_BRANCH}"
SHOW_REF_SHA_MAIN=""
SHOW_REF_SHA_DEV=""
SHOW_REF_SHA_TAG=""
SHOW_REF_SHA_NESTED=""
SHOW_REF_SHA_DETACHED="eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"

init_repo() {
    "$QGIT_BIN" init -q
}

run_show_ref() {
    run "$QGIT_BIN" show-ref "$@"
}

git_show_ref() {
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" show-ref "$@"
}

make_commit() {
    local msg="${1:-test commit}"
    local blob_file="$TEST_DIR/show-ref-blob-$$.txt"
    local commit_file="$TEST_DIR/show-ref-commit-$$.txt"
    local git_dir blob_sha tree_sha commit_sha

    git_dir=$(qgit_meta_dir)
    printf '%s\n' "$msg" >"$blob_file"
    blob_sha=$(env GIT_DIR="$git_dir" "$GIT" hash-object -w "$blob_file")
    tree_sha=$(printf '100644 blob %s\tfile.txt\n' "$blob_sha" | env GIT_DIR="$git_dir" "$GIT" mktree)
    cat >"$commit_file" <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

$msg
EOF
    commit_sha=$(env GIT_DIR="$git_dir" "$GIT" hash-object -w -t commit "$commit_file")
    rm -f "$blob_file" "$commit_file"
    printf '%s\n' "$commit_sha"
}

write_ref() {
    local refname="$1"
    local sha="$2"
    local file

    file="$(qgit_meta_dir)/$refname"
    mkdir -p "$(dirname "$file")"
    printf '%s\n' "$sha" >"$file"
}

set_symref_head() {
    local branch="$1"

    printf 'ref: refs/heads/%s\n' "$branch" >"$(qgit_meta_dir)/HEAD"
}

set_detached_head() {
    local sha="$1"

    printf '%s\n' "$sha" >"$(qgit_meta_dir)/HEAD"
}

assert_sorted_output_equals() {
    local expected="$1"
    local expected_sorted actual_sorted

    expected_sorted=$(printf '%s\n' "$expected" | sed '/^$/d' | sort)
    actual_sorted=$(printf '%s\n' "$output" | sed '/^$/d' | sort)
    if [ "$expected_sorted" != "$actual_sorted" ]; then
        echo "Expected (sorted):"
        echo "$expected_sorted"
        echo "Actual (sorted):"
        echo "$actual_sorted"
        return 1
    fi
}

assert_matches_git_show_ref() {
    local expected

    expected=$(git_show_ref "$@")
    run_show_ref "$@"
    assert_success
    assert_sorted_output_equals "$expected"
}

ref_line() {
    printf '%s %s' "$1" "$2"
}

setup_basic_refs() {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    SHOW_REF_SHA_DEV=$(make_commit "dev commit")
    SHOW_REF_SHA_TAG=$(make_commit "tag commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    write_ref "refs/heads/dev" "$SHOW_REF_SHA_DEV"
    write_ref "refs/tags/v1.0.0" "$SHOW_REF_SHA_TAG"
}

# show-ref

@test "qgit show-ref: outside repository fails" {
    run_show_ref
    assert_failure
}

@test "qgit show-ref: fresh repository produces no output" {
    init_repo
    run_show_ref
    assert_success
    assert_output_empty
}

@test "qgit show-ref: lists branch references by default" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    run_show_ref
    assert_success
    assert_output_equals "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
}

@test "qgit show-ref: lists tag references by default" {
    init_repo
    SHOW_REF_SHA_TAG=$(make_commit "tag commit")
    write_ref "refs/tags/v1.0.0" "$SHOW_REF_SHA_TAG"
    run_show_ref
    assert_success
    assert_output_equals "$(ref_line "$SHOW_REF_SHA_TAG" "refs/tags/v1.0.0")"
}

@test "qgit show-ref: lists branches and tags together" {
    setup_basic_refs
    run_show_ref
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_DEV" "refs/heads/dev")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_TAG" "refs/tags/v1.0.0")"
}

@test "qgit show-ref: default output omits HEAD" {
    setup_basic_refs
    set_symref_head "$SHOW_REF_BRANCH"
    run_show_ref
    assert_success
    assert_output_not_contains " HEAD"
    assert_output_not_contains "^HEAD "
}

@test "qgit show-ref: matches git output for loose refs" {
    setup_basic_refs
    assert_matches_git_show_ref
}

@test "qgit show-ref: works when tags directory is missing" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    rmdir "$(qgit_meta_dir)/refs/tags"
    run_show_ref
    assert_success
    assert_output_equals "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
}

# --head

@test "qgit show-ref --head: prints resolved HEAD for symref" {
    setup_basic_refs
    set_symref_head "$SHOW_REF_BRANCH"
    run_show_ref --head
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "HEAD")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
}

@test "qgit show-ref --head: prints detached HEAD directly" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    set_detached_head "$SHOW_REF_SHA_DETACHED"
    run_show_ref --head
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_DETACHED" "HEAD")"
    assert_output_not_contains "$(ref_line "$SHOW_REF_SHA_DETACHED" "refs/heads/$SHOW_REF_BRANCH")"
}

@test "qgit show-ref --head: matches git output" {
    setup_basic_refs
    set_symref_head "$SHOW_REF_BRANCH"
    assert_matches_git_show_ref --head
}

@test "qgit show-ref --head: fails when symref target is missing" {
    init_repo
    set_symref_head "$SHOW_REF_BRANCH"
    run_show_ref --head
    assert_failure
}

# --branches

@test "qgit show-ref --branches: lists branches only" {
    setup_basic_refs
    run_show_ref --branches
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_DEV" "refs/heads/dev")"
    assert_output_not_contains "refs/tags/"
}

@test "qgit show-ref --branches: matches git output" {
    setup_basic_refs
    assert_matches_git_show_ref --branches
}

# --tags

@test "qgit show-ref --tags: lists tags only" {
    setup_basic_refs
    run_show_ref --tags
    assert_success
    assert_output_equals "$(ref_line "$SHOW_REF_SHA_TAG" "refs/tags/v1.0.0")"
    assert_output_not_contains "refs/heads/"
}

@test "qgit show-ref --tags: matches git output" {
    setup_basic_refs
    assert_matches_git_show_ref --tags
}

@test "qgit show-ref --tags: empty when no tags exist" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    run_show_ref --tags
    assert_success
    assert_output_empty
}

# --branches --tags

@test "qgit show-ref --branches --tags: lists both namespaces" {
    setup_basic_refs
    run_show_ref --branches --tags
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_DEV" "refs/heads/dev")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_TAG" "refs/tags/v1.0.0")"
}

@test "qgit show-ref --branches --tags: matches git output" {
    setup_basic_refs
    assert_matches_git_show_ref --branches --tags
}

# nested refs

@test "qgit show-ref: lists nested branch names" {
    init_repo
    SHOW_REF_SHA_NESTED=$(make_commit "nested branch commit")
    write_ref "refs/heads/feature/nested" "$SHOW_REF_SHA_NESTED"
    run_show_ref
    assert_success
    assert_output_equals "$(ref_line "$SHOW_REF_SHA_NESTED" "refs/heads/feature/nested")"
}

@test "qgit show-ref --branches: lists nested branch with other branches" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    SHOW_REF_SHA_NESTED=$(make_commit "nested branch commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    write_ref "refs/heads/feature/nested" "$SHOW_REF_SHA_NESTED"
    run_show_ref --branches
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_NESTED" "refs/heads/feature/nested")"
}

@test "qgit show-ref: nested branch matches git output" {
    init_repo
    SHOW_REF_SHA_NESTED=$(make_commit "nested branch commit")
    write_ref "refs/heads/feature/nested" "$SHOW_REF_SHA_NESTED"
    assert_matches_git_show_ref
}

# output format

@test "qgit show-ref: prints oid and refname separated by space" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    run_show_ref
    assert_success
    assert_output_matches "^[0-9a-f]{40} refs/heads/${SHOW_REF_BRANCH}$"
}

# -h

@test "qgit show-ref -h: shows help" {
    run_show_ref -h
    assert_success
    assert_output_contains "qgit show-ref"
    assert_output_contains "--head"
    assert_output_contains "--branches"
    assert_output_contains "--tags"
}

@test "qgit show-ref --help: shows help" {
    run_show_ref --help
    assert_success
    assert_output_contains "qgit show-ref"
    assert_output_contains "List references in a local repository"
}

# errors

@test "qgit show-ref: unknown flag fails" {
    init_repo
    run_show_ref --unknown-flag
    assert_failure
}
