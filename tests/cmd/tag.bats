load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

# helpers

TAG_BRANCH="${QGIT_DEFAULT_BRANCH}"

init_repo() {
    "$QGIT_BIN" init -q
}

run_tag() {
    run "$QGIT_BIN" tag "$@"
}

git_tag() {
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" tag "$@"
}

make_commit() {
    local msg="${1:-test commit}"
    local blob_file="$TEST_DIR/tag-blob-$$.txt"
    local commit_file="$TEST_DIR/tag-commit-$$.txt"
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

setup_branch_head() {
    local sha

    init_repo
    sha=$(make_commit "$1")
    write_ref "refs/heads/$TAG_BRANCH" "$sha"
    set_symref_head "$TAG_BRANCH"
    printf '%s\n' "$sha"
}

tag_ref_file() {
    local name="$1"

    printf '%s/refs/tags/%s\n' "$(qgit_meta_dir)" "$name"
}

assert_tag_ref_equals() {
    local name="$1"
    local sha="$2"
    local file content

    file=$(tag_ref_file "$name")
    assert_file_exists "$file"
    content=$(cat "$file")
    if [ "$content" != "$sha" ]; then
        echo "Expected tag ref: $sha"
        echo "Actual tag ref: $content"
        return 1
    fi
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

assert_matches_git_tag_list() {
    local expected

    expected=$(git_tag "$@")
    run_tag "$@"
    assert_success
    assert_sorted_output_equals "$expected"
}

# tag

@test "qgit tag: outside repository fails" {
    run_tag v1.0.0
    assert_failure
}

@test "qgit tag: create lightweight tag at HEAD" {
    local sha

    sha=$(setup_branch_head "head commit")
    run_tag v1.0.0
    assert_success
    assert_output_empty
    assert_tag_ref_equals "v1.0.0" "$sha"
}

@test "qgit tag: create tag at explicit commit" {
    local sha other

    sha=$(setup_branch_head "target commit")
    other=$(make_commit "other commit")
    run_tag release "$other"
    assert_success
    assert_output_empty
    assert_tag_ref_equals "release" "$other"
}

@test "qgit tag: create tag at explicit commit hash" {
    local sha

    sha=$(setup_branch_head "hash target")
    run_tag annotated "$sha"
    assert_success
    assert_tag_ref_equals "annotated" "$sha"
}

@test "qgit tag: create tag using branch name as commit" {
    local sha

    sha=$(setup_branch_head "branch target")
    run_tag from-branch "$TAG_BRANCH"
    assert_success
    assert_tag_ref_equals "from-branch" "$sha"
}

@test "qgit tag: overwrite existing tag" {
    local sha first second

    sha=$(setup_branch_head "first commit")
    run_tag stable "$sha"
    assert_success
    second=$(make_commit "second commit")
    run_tag stable "$second"
    assert_success
    assert_tag_ref_equals "stable" "$second"
}

@test "qgit tag: create nested tag name" {
    local sha

    sha=$(setup_branch_head "nested commit")
    run_tag release/v1.0.0
    assert_success
    assert_tag_ref_equals "release/v1.0.0" "$sha"
}

@test "qgit tag: matches git create and list" {
    local sha

    sha=$(setup_branch_head "git parity")
    git_tag v1.0.0 "$sha"
    run_tag v2.0.0 "$sha"
    assert_success
    assert_matches_git_tag_list
}

@test "qgit tag: fails when HEAD does not resolve" {
    init_repo
    run_tag v1.0.0
    assert_failure
}

@test "qgit tag: fails with unresolvable commit" {
    setup_branch_head "known commit"
    run_tag v1.0.0 no-such-branch
    assert_failure
}

# -d

@test "qgit tag -d: delete existing tag" {
    local sha

    sha=$(setup_branch_head "delete me")
    run_tag v1.0.0 "$sha"
    assert_success
    run_tag -d v1.0.0
    assert_success
    assert_output_empty
    [ ! -f "$(tag_ref_file v1.0.0)" ]
}

@test "qgit tag --delete: delete existing tag" {
    local sha

    sha=$(setup_branch_head "delete me")
    run_tag v1.0.0 "$sha"
    assert_success
    run_tag --delete v1.0.0
    assert_success
    [ ! -f "$(tag_ref_file v1.0.0)" ]
}

@test "qgit tag -d: delete nested tag" {
    local sha

    sha=$(setup_branch_head "nested delete")
    run_tag release/v1.0.0 "$sha"
    assert_success
    run_tag -d release/v1.0.0
    assert_success
    [ ! -f "$(tag_ref_file release/v1.0.0)" ]
}

@test "qgit tag -d: missing tag name fails" {
    setup_branch_head "delete setup"
    run_tag -d
    assert_failure
}

@test "qgit tag -d: missing tag fails" {
    setup_branch_head "delete setup"
    run_tag -d missing-tag
    assert_failure
}

# list

@test "qgit tag: list with no tags produces no output" {
    init_repo
    run_tag
    assert_success
    assert_output_empty
}

@test "qgit tag: list all tags" {
    local sha

    sha=$(setup_branch_head "list commit")
    run_tag alpha "$sha"
    assert_success
    run_tag beta "$sha"
    assert_success
    run_tag
    assert_success
    assert_output_contains "alpha"
    assert_output_contains "beta"
}

@test "qgit tag -l: list all tags" {
    local sha

    sha=$(setup_branch_head "list commit")
    run_tag alpha "$sha"
    assert_success
    run_tag beta "$sha"
    assert_success
    run_tag -l
    assert_success
    assert_output_contains "alpha"
    assert_output_contains "beta"
}

@test "qgit tag --list: list all tags" {
    local sha

    sha=$(setup_branch_head "list commit")
    run_tag alpha "$sha"
    assert_success
    run_tag --list
    assert_success
    assert_output_contains "alpha"
}

@test "qgit tag: list prints tag names only" {
    local sha

    sha=$(setup_branch_head "list commit")
    run_tag release/v1.0.0 "$sha"
    assert_success
    run_tag
    assert_success
    assert_output_equals "release/v1.0.0"
    assert_output_not_contains "refs/tags/"
}

@test "qgit tag: list matches git output" {
    local sha

    sha=$(setup_branch_head "git list")
    git_tag alpha "$sha"
    git_tag beta "$sha"
    assert_matches_git_tag_list
}

@test "qgit tag: list works when tags directory is missing" {
    local sha

    sha=$(setup_branch_head "missing tags dir")
    rmdir "$(qgit_meta_dir)/refs/tags"
    run_tag
    assert_success
    assert_output_empty
}

# -h

@test "qgit tag -h: shows help" {
    run_tag -h
    assert_success
    assert_output_contains "qgit tag"
    assert_output_contains "-d"
    assert_output_contains "-l"
    assert_output_contains "<tagname>"
}

@test "qgit tag --help: shows help" {
    run_tag --help
    assert_success
    assert_output_contains "qgit tag"
    assert_output_contains "Create, list, or delete tags"
}

# errors

@test "qgit tag: unknown flag fails" {
    init_repo
    run_tag --unknown-flag
    assert_failure
}
