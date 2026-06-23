TAG_BRANCH="${QGIT_DEFAULT_BRANCH}"
TAG_USER_NAME="Test User"
TAG_USER_EMAIL="test@example.com"
TAG_ANNOTATED_MESSAGE="Release version 1.0"

init_repo() {
    "$QGIT_BIN" init -q
}

setup_user_identity() {
    "$QGIT_BIN" config --set user.name "$TAG_USER_NAME"
    "$QGIT_BIN" config --set user.email "$TAG_USER_EMAIL"
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

git_rev_parse() {
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" rev-parse "$@"
}

setup_git_identity() {
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" config user.name "$TAG_USER_NAME"
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" config user.email "$TAG_USER_EMAIL"
}

setup_git_annotated_tag() {
    local tagname="$1"
    local commit_sha="$2"
    local message="${3:-$TAG_ANNOTATED_MESSAGE}"
    local tag_obj_sha

    setup_git_identity
    git_tag -a "$tagname" -m "$message" "$commit_sha"
    tag_obj_sha=$(git_rev_parse "$tagname")
    printf '%s\n' "$commit_sha" "$tag_obj_sha"
}

setup_qgit_annotated_tag() {
    local tagname="$1"
    local commit_sha="$2"
    local message="${3:-$TAG_ANNOTATED_MESSAGE}"

    setup_user_identity
    run_tag -a -m "$message" "$tagname" "$commit_sha"
    assert_success
    assert_output_empty
}

assert_tag_ref_not_equals() {
    local name="$1"
    local sha="$2"
    local actual

    actual=$(read_tag_ref "$name")
    if [ "$actual" = "$sha" ]; then
        echo "Expected tag ref to differ from: $sha"
        echo "Actual tag ref: $actual"
        return 1
    fi
}

assert_tag_object_type() {
    local tag_ref_sha="$1"
    local type

    type=$("$QGIT_BIN" cat-file -t "$tag_ref_sha")
    if [ "$type" != "tag" ]; then
        echo "Expected tag object type, got: $type"
        return 1
    fi
}

assert_annotated_tag_points_at_object() {
    local tagname="$1"
    local object_sha="$2"
    local object_type="$3"
    local tag_ref_sha object_sha_actual object_type_actual

    tag_ref_sha=$(read_tag_ref "$tagname")
    assert_tag_ref_not_equals "$tagname" "$object_sha"
    assert_tag_object_type "$tag_ref_sha"
    object_sha_actual=$("$QGIT_BIN" cat-file tag "$tag_ref_sha" | awk '/^object / { print $2; exit }')
    object_type_actual=$("$QGIT_BIN" cat-file tag "$tag_ref_sha" | awk '/^type / { print $2; exit }')
    if [ "$object_sha_actual" != "$object_sha" ]; then
        echo "Expected annotated tag object to point at: $object_sha"
        echo "Actual object line: $object_sha_actual"
        return 1
    fi
    if [ "$object_type_actual" != "$object_type" ]; then
        echo "Expected annotated tag type line: $object_type"
        echo "Actual type line: $object_type_actual"
        return 1
    fi
}

assert_annotated_tag_points_at_commit() {
    assert_annotated_tag_points_at_object "$1" "$2" "commit"
}

assert_tag_message_in_object() {
    local tagname="$1"
    local message="$2"
    local tag_sha

    tag_sha=$(read_tag_ref "$tagname")
    run "$QGIT_BIN" cat-file -p "$tag_sha"
    assert_success
    assert_output_contains "$message"
}

make_blob() {
    local content="${1:-blob content}"
    local blob_file="$TEST_DIR/tag-blob-only-$$.txt"
    local blob_sha

    printf '%s\n' "$content" >"$blob_file"
    blob_sha=$(env GIT_DIR="$(qgit_meta_dir)" "$GIT" hash-object -w "$blob_file")
    rm -f "$blob_file"
    printf '%s\n' "$blob_sha"
}

make_empty_tree() {
    local tree_sha

    tree_sha=$(printf '' | env GIT_DIR="$(qgit_meta_dir)" "$GIT" mktree)
    printf '%s\n' "$tree_sha"
}

make_tree() {
    local blob_sha tree_sha

    blob_sha=$(make_blob)
    tree_sha=$(printf '100644 blob %s\tfile.txt\n' "$blob_sha" | env GIT_DIR="$(qgit_meta_dir)" "$GIT" mktree)
    printf '%s\n' "$tree_sha"
}

make_annotated_tag_object() {
    local object_sha="$1"
    local object_type="$2"
    local tag_name="${3:-base-tag}"
    local message="${4:-Base tag message}"
    local tag_file tag_obj_sha

    tag_file="$TEST_DIR/tag-object-$$.txt"
    cat >"$tag_file" <<EOF
object $object_sha
type $object_type
tag $tag_name
tagger Test User <test@example.com> 946684800 +0000

$message
EOF
    tag_obj_sha=$(env GIT_DIR="$(qgit_meta_dir)" "$GIT" hash-object -w -t tag "$tag_file")
    rm -f "$tag_file"
    printf '%s\n' "$tag_obj_sha"
}

assert_git_annotated_tag_points_at_object() {
    local tagname="$1"
    local object_sha="$2"
    local object_type="$3"
    local tag_ref_sha object_sha_actual object_type_actual

    tag_ref_sha=$(git_rev_parse "$tagname")
    object_sha_actual=$(env GIT_DIR="$(qgit_meta_dir)" "$GIT" cat-file tag "$tag_ref_sha" | awk '/^object / { print $2; exit }')
    object_type_actual=$(env GIT_DIR="$(qgit_meta_dir)" "$GIT" cat-file tag "$tag_ref_sha" | awk '/^type / { print $2; exit }')
    if [ "$object_sha_actual" != "$object_sha" ]; then
        echo "Expected git annotated tag to point at: $object_sha"
        echo "Actual object line: $object_sha_actual"
        return 1
    fi
    if [ "$object_type_actual" != "$object_type" ]; then
        echo "Expected git annotated tag type line: $object_type"
        echo "Actual type line: $object_type_actual"
        return 1
    fi
}

assert_matches_git_annotated_tag_on_object() {
    local tagname="$1"
    local object_sha="$2"
    local object_type="$3"
    local message="${4:-$TAG_ANNOTATED_MESSAGE}"
    local git_tagname="git-$tagname"

    setup_git_identity
    git_tag -a "$git_tagname" -m "$message" "$object_sha"
    assert_git_annotated_tag_points_at_object "$git_tagname" "$object_sha" "$object_type"

    setup_user_identity
    run_tag -a -m "$message" "$tagname" "$object_sha"
    assert_success
    assert_output_empty
    assert_annotated_tag_points_at_object "$tagname" "$object_sha" "$object_type"
}
