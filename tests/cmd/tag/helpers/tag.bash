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

read_tag_ref() {
    tr -d ' \n\r' <"$(tag_ref_file "$1")"
}

assert_tag_ref_equals() {
    local name="$1"
    local sha="$2"
    local actual

    assert_file_exists "$(tag_ref_file "$name")"
    actual=$(read_tag_ref "$name")
    if [ "$actual" != "$sha" ]; then
        echo "Expected tag ref: $sha"
        echo "Actual tag ref: $actual"
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

assert_ordered_output_equals() {
    local expected="$1"
    local expected_trimmed actual_trimmed

    expected_trimmed=$(printf '%s\n' "$expected" | sed '/^$/d')
    actual_trimmed=$(printf '%s\n' "$output" | sed '/^$/d')
    if [ "$expected_trimmed" != "$actual_trimmed" ]; then
        echo "Expected (ordered):"
        printf '%s\n' "$expected_trimmed"
        echo "Actual (ordered):"
        printf '%s\n' "$actual_trimmed"
        return 1
    fi
}

assert_matches_git_tag_list_ordered() {
    local expected

    expected=$(git_tag "$@")
    run_tag "$@"
    assert_success
    assert_ordered_output_equals "$expected"
}

assert_matches_git_tag_list() {
    local expected

    expected=$(git_tag "$@")
    run_tag "$@"
    assert_success
    assert_sorted_output_equals "$expected"
}

assert_matches_git_tag_create() {
    local tagname="$1"
    local commit="$2"
    local expected_sha

    expected_sha=$(env GIT_DIR="$(qgit_meta_dir)" "$GIT" rev-parse "$commit")
    run_tag "$tagname" "$commit"
    assert_success
    assert_output_empty
    assert_tag_ref_equals "$tagname" "$expected_sha"
}
