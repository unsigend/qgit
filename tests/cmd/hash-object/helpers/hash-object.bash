EMPTY_BLOB_SHA="e69de29bb2d1d6434b8b29ae775ad8c2e48c5391"
EMPTY_TREE_SHA="4b825dc642cb6eb9a060e54bf8d69288fbee4904"

init_repo() {
    "$QGIT_BIN" init -q
}

run_hash_object() {
    run "$QGIT_BIN" hash-object "$@"
}

git_hash_object() {
    "$GIT" hash-object "$@"
}

object_path() {
    local sha="$1"
    local meta="${2:-$(qgit_meta_dir)}"

    printf '%s/objects/%s/%s\n' "$meta" "${sha:0:2}" "${sha:2}"
}

assert_object_exists() {
    local sha="$1"

    assert_file_exists "$(object_path "$sha")"
}

assert_object_absent() {
    local sha="$1"
    local path

    path=$(object_path "$sha")
    if [ -f "$path" ]; then
        echo "Expected object to not exist: $path"
        return 1
    fi
}

assert_matches_git_hash() {
    local expected

    expected=$(git_hash_object "$@")
    run_hash_object "$@"
    assert_success
    assert_output_equals "$expected"
}

assert_matches_git_hash_in_repo() {
    local expected hash_args=() arg

    for arg in "$@"; do
        case "$arg" in
        -w|--write)
            ;;
        *)
            hash_args+=("$arg")
            ;;
        esac
    done

    expected=$(git_hash_object "${hash_args[@]}")
    run_hash_object "$@"
    assert_success
    assert_output_equals "$expected"
}

git_hash_args_from_qgit() {
    local git_args=()
    local arg

    for arg in "$@"; do
        case "$arg" in
        --type)
            git_args+=(-t)
            ;;
        --write)
            git_args+=(-w)
            ;;
        *)
            git_args+=("$arg")
            ;;
        esac
    done

    printf '%s\n' "${git_args[@]}"
}

assert_qgit_matches_git_hash() {
    local expected git_args

    git_args=$(git_hash_args_from_qgit "$@")
    # shellcheck disable=SC2086
    expected=$(git_hash_object $git_args)
    run_hash_object "$@"
    assert_success
    assert_output_equals "$expected"
}

assert_qgit_matches_git_hash_in_repo() {
    local expected hash_args=() arg

    for arg in "$@"; do
        case "$arg" in
        -w|--write)
            ;;
        --type)
            hash_args+=(-t)
            ;;
        *)
            hash_args+=("$arg")
            ;;
        esac
    done

    expected=$(git_hash_object "${hash_args[@]}")
    run_hash_object "$@"
    assert_success
    assert_output_equals "$expected"
}

write_valid_tree_file() {
    local file="$1"
    local tree_sha fixture empty_blob

    fixture="$TEST_DIR/tree-fixture"
    mkdir -p "$fixture"
    env GIT_DIR="$fixture/.git" "$GIT" init -q "$fixture"
    empty_blob="$fixture/empty.blob"
    : >"$empty_blob"
    env GIT_DIR="$fixture/.git" "$GIT" hash-object -w "$empty_blob"
    tree_sha=$(printf '100644 blob %s\tfile.txt\n' "$EMPTY_BLOB_SHA" | \
        env GIT_DIR="$fixture/.git" "$GIT" mktree)
    env GIT_DIR="$fixture/.git" "$GIT" cat-file tree "$tree_sha" >"$file"
}

write_valid_commit_file() {
    local file="$1"

    cat >"$file" <<EOF
tree $EMPTY_TREE_SHA
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

test commit message
EOF
}

write_valid_tag_file() {
    local file="$1"
    local object_sha="$2"

    cat >"$file" <<EOF
object $object_sha
type commit
tag test-tag
tagger Test User <test@example.com> 946684800 +0000

tag message
EOF
}
