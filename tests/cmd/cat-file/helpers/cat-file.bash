init_repo() {
    "$QGIT_BIN" init -q
}

run_cat_file() {
    run "$QGIT_BIN" cat-file "$@"
}

git_cat() {
    env GIT_OBJECT_DIRECTORY="$TEST_DIR/.qgit/objects" GIT_DIR="$TEST_DIR/.qgit" "$GIT" cat-file "$@"
}

normalize_cat_output() {
    printf '%s' "$1" | tr -d ' \t\r'
}

assert_output_normalized_equals() {
    local expected="$1"
    local actual="$2"
    local norm_exp norm_act

    norm_exp="$(normalize_cat_output "$expected")"
    norm_act="$(normalize_cat_output "$actual")"
    if [ "$norm_exp" != "$norm_act" ]; then
        echo "Expected (normalized): $norm_exp"
        echo "Actual (normalized):   $norm_act"
        return 1
    fi
}

assert_matches_git_cat() {
    local expected
    expected=$(git_cat "$@")
    run_cat_file "$@"
    assert_success
    assert_output_equals "$expected"
}

assert_matches_git_cat_p() {
    local hash="$1"
    local expected

    expected=$(git_cat -p "$hash")
    run_cat_file -p "$hash"
    assert_success
    assert_output_normalized_equals "$expected" "$output"
}

init_mock_git() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" init -q
    env GIT_DIR="$TEST_DIR/.git" "$GIT" config user.name "Test User"
    env GIT_DIR="$TEST_DIR/.git" "$GIT" config user.email "test@example.com"
}

sync_git_to_qgit() {
    rm -rf "$TEST_DIR/.qgit"
    cp -r "$TEST_DIR/.git" "$TEST_DIR/.qgit"
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

git_write_tag() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" hash-object -w -t tag "$@"
}

EMPTY_TREE_SHA="4b825dc642cb6eb9a060e54bf8d69288fbee4904"

write_ref() {
    local refname="$1"
    local sha="$2"
    local file

    file="$TEST_DIR/.qgit/$refname"
    mkdir -p "$(dirname "$file")"
    printf '%s\n' "$sha" >"$file"
}

set_symref_head() {
    local branch="$1"

    printf 'ref: refs/heads/%s\n' "$branch" >"$TEST_DIR/.qgit/HEAD"
}

abbrev_sha() {
    local sha="$1"
    local len="$2"

    printf '%s' "${sha:0:$len}"
}
