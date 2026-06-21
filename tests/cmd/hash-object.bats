load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

# helpers

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

# hash-object

@test "qgit hash-object: hashes blob file and prints oid" {
    printf 'hello world' >payload.txt

    run_hash_object payload.txt
    assert_success
    assert_output_matches '^[0-9a-f]{40}$'
}

@test "qgit hash-object: default type is blob" {
    printf 'hello world' >payload.txt

    assert_matches_git_hash payload.txt
}

@test "qgit hash-object: matches git output for text file" {
    printf 'line one\nline two\n' >payload.txt

    assert_matches_git_hash payload.txt
}

@test "qgit hash-object: matches git output without trailing newline" {
    printf 'no newline' >payload.txt

    assert_matches_git_hash payload.txt
}

@test "qgit hash-object: empty file matches git empty blob hash" {
    : >empty.txt

    assert_matches_git_hash empty.txt
    assert_output_equals "$EMPTY_BLOB_SHA"
}

@test "qgit hash-object: works outside a repository without -w" {
    assert_qgit_absent
    printf 'outside repo' >payload.txt

    assert_matches_git_hash payload.txt
}

@test "qgit hash-object: prints oid followed by newline only" {
    printf 'payload' >payload.txt

    run_hash_object payload.txt
    assert_success
    assert_output_matches '^[0-9a-f]{40}$'
}

# -t

@test "qgit hash-object -t blob: matches git output" {
    printf 'typed blob' >payload.txt

    assert_matches_git_hash -t blob payload.txt
}

@test "qgit hash-object --type blob: matches git output" {
    printf 'typed blob' >payload.txt

    assert_qgit_matches_git_hash --type blob payload.txt
}

@test "qgit hash-object -t tree: matches git for valid tree content" {
    write_valid_tree_file tree.txt

    assert_matches_git_hash -t tree tree.txt
}

@test "qgit hash-object -t commit: matches git for valid commit content" {
    write_valid_commit_file commit.txt

    assert_matches_git_hash -t commit commit.txt
}

@test "qgit hash-object -t tag: matches git for valid tag content" {
    local commit_sha

    write_valid_commit_file commit.txt
    commit_sha=$(git_hash_object -t commit commit.txt)
    write_valid_tag_file tag.txt "$commit_sha"

    assert_matches_git_hash -t tag tag.txt
}

@test "qgit hash-object --type commit: matches git output" {
    write_valid_commit_file commit.txt

    assert_qgit_matches_git_hash --type commit commit.txt
}

# -w

@test "qgit hash-object -w: writes loose object into repository" {
    init_repo
    printf 'store me' >payload.txt

    run_hash_object -w payload.txt
    assert_success
    assert_object_exists "$output"
}

@test "qgit hash-object -w: oid matches git hash-object -w" {
    init_repo
    printf 'store me' >payload.txt

    assert_matches_git_hash_in_repo -w payload.txt
}

@test "qgit hash-object --write: matches git output and stores object" {
    init_repo
    printf 'long write flag' >payload.txt

    assert_qgit_matches_git_hash_in_repo --write payload.txt
    assert_object_exists "$output"
}

@test "qgit hash-object: without -w does not store object" {
    init_repo
    printf 'no write' >payload.txt

    run_hash_object payload.txt
    assert_success
    assert_object_absent "$output"
}

@test "qgit hash-object -w: stores empty blob object" {
    init_repo
    : >empty.txt

    run_hash_object -w empty.txt
    assert_success
    assert_output_equals "$EMPTY_BLOB_SHA"
    assert_object_exists "$EMPTY_BLOB_SHA"
}

@test "qgit hash-object -w -t commit: stores commit object" {
    init_repo
    write_valid_commit_file commit.txt

    assert_matches_git_hash_in_repo -w -t commit commit.txt
    assert_object_exists "$output"
}

@test "qgit hash-object -w -t tree: stores tree object" {
    init_repo
    write_valid_tree_file tree.txt

    assert_matches_git_hash_in_repo -w -t tree tree.txt
    assert_object_exists "$output"
}

@test "qgit hash-object -w: repeated write produces same oid" {
    init_repo
    printf 'repeat' >payload.txt

    run_hash_object -w payload.txt
    assert_success
    first="$output"

    run_hash_object -w payload.txt
    assert_success
    assert_output_equals "$first"
    assert_object_exists "$first"
}

@test "qgit hash-object -w: fails outside a repository" {
    assert_qgit_absent
    printf 'orphan' >payload.txt

    run_hash_object -w payload.txt
    assert_failure
}

# path

@test "qgit hash-object <file>: nested path works" {
    mkdir -p nested/dir
    printf 'nested payload' >nested/dir/payload.txt

    assert_matches_git_hash nested/dir/payload.txt
}

@test "qgit hash-object <file>: missing file fails" {
    run_hash_object missing.txt
    assert_failure
}

# -h

@test "qgit hash-object -h: shows help" {
    run_hash_object -h
    assert_success
    assert_output_contains "qgit hash-object"
    assert_output_contains "-t"
    assert_output_contains "--write"
    assert_output_contains "<file>"
}

@test "qgit hash-object --help: shows help" {
    run_hash_object --help
    assert_success
    assert_output_contains "qgit hash-object"
    assert_output_contains "Compute object ID"
    assert_output_contains "specify the type of the object"
}

@test "qgit hash-object -h: does not require file argument" {
    run_hash_object -h
    assert_success
}

# errors

@test "qgit hash-object: missing file argument fails" {
    run_hash_object
    assert_failure
}

@test "qgit hash-object -t: missing type argument fails" {
    printf 'payload' >payload.txt

    run_hash_object -t payload.txt
    assert_failure
}

@test "qgit hash-object -t: invalid type fails" {
    printf 'payload' >payload.txt

    run_hash_object -t invalid payload.txt
    assert_failure
}

@test "qgit hash-object: unknown option fails" {
    printf 'payload' >payload.txt

    run_hash_object --stdin payload.txt
    assert_failure
}

@test "qgit hash-object: directory path fails" {
    mkdir payload.dir

    run_hash_object payload.dir
    assert_failure
}
