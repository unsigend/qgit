SHOW_REF_BRANCH="${QGIT_DEFAULT_BRANCH}"
SHOW_REF_SHA_MAIN=""
SHOW_REF_SHA_DEV=""
SHOW_REF_SHA_TAG=""
SHOW_REF_SHA_TAG2=""
SHOW_REF_SHA_NESTED=""
SHOW_REF_SHA_DETACHED="eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"

init_repo() {
    "$QGIT_BIN" init -q
}

run_show_ref() {
    run "$QGIT_BIN" show-ref "$@"
}

git_show_ref() {
    local args=()
    local arg

    for arg in "$@"; do
        case "$arg" in
            --branches)
                args+=(--heads)
                ;;
            *)
                args+=("$arg")
                ;;
        esac
    done
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" show-ref "${args[@]}"
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

assert_matches_git_show_ref_order() {
    local expected

    expected=$(git_show_ref "$@")
    run_show_ref "$@"
    assert_success
    assert_output_equals "$expected"
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

setup_multi_tag_refs() {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    SHOW_REF_SHA_TAG=$(make_commit "tag one")
    SHOW_REF_SHA_TAG2=$(make_commit "tag two")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    write_ref "refs/tags/v1.0.0" "$SHOW_REF_SHA_TAG"
    write_ref "refs/tags/release/v2.0.0" "$SHOW_REF_SHA_TAG2"
}

# Write refs in reverse lexicographic order so readdir order differs from git.
setup_out_of_order_refs() {
    init_repo
    SHOW_REF_SHA_ZEBRA=$(make_commit "zebra commit")
    SHOW_REF_SHA_MIDDLE=$(make_commit "middle commit")
    SHOW_REF_SHA_ALPHA=$(make_commit "alpha commit")
    SHOW_REF_SHA_TAG_Z=$(make_commit "tag z commit")
    SHOW_REF_SHA_TAG_A=$(make_commit "tag a commit")
    write_ref "refs/heads/zebra" "$SHOW_REF_SHA_ZEBRA"
    write_ref "refs/heads/middle" "$SHOW_REF_SHA_MIDDLE"
    write_ref "refs/heads/alpha" "$SHOW_REF_SHA_ALPHA"
    write_ref "refs/tags/z-release" "$SHOW_REF_SHA_TAG_Z"
    write_ref "refs/tags/a-release" "$SHOW_REF_SHA_TAG_A"
}

setup_nested_out_of_order_refs() {
    init_repo
    SHOW_REF_SHA_TRUNK=$(make_commit "trunk commit")
    SHOW_REF_SHA_NESTED_Z=$(make_commit "nested z commit")
    SHOW_REF_SHA_NESTED_A=$(make_commit "nested a commit")
    write_ref "refs/heads/trunk" "$SHOW_REF_SHA_TRUNK"
    write_ref "refs/heads/feature/zeta" "$SHOW_REF_SHA_NESTED_Z"
    write_ref "refs/heads/feature/alpha" "$SHOW_REF_SHA_NESTED_A"
}

setup_git_identity() {
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" config user.name "Test User"
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" config user.email "test@example.com"
}

git_tag() {
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" tag "$@"
}

git_rev_parse() {
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" rev-parse "$@"
}

setup_annotated_tag_ref() {
    local tagname="${1:-v1.0.0}"
    local message="${2:-Release version 1.0}"
    local commit_sha tag_obj_sha

    init_repo
    commit_sha=$(make_commit "annotated tag commit")
    setup_git_identity
    git_tag -a "$tagname" -m "$message" "$commit_sha"
    tag_obj_sha=$(git_rev_parse "$tagname")
    SHOW_REF_SHA_ANNOTATED_COMMIT="$commit_sha"
    SHOW_REF_SHA_ANNOTATED_TAG="$tag_obj_sha"
    printf '%s\n' "$commit_sha" "$tag_obj_sha"
}

setup_mixed_lightweight_annotated_refs() {
    local commit_sha tag_obj_sha commit_sha2 tag_obj_sha2

    init_repo
    commit_sha=$(make_commit "mixed tag commit one")
    commit_sha2=$(make_commit "mixed tag commit two")
    setup_git_identity
    git_tag -a v1.0.0 -m "Release version 1.0" "$commit_sha"
    tag_obj_sha=$(git_rev_parse v1.0.0)
    write_ref "refs/tags/lightweight" "$commit_sha2"
    SHOW_REF_SHA_MAIN=$(make_commit "mixed refs main")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    SHOW_REF_SHA_TAG="$tag_obj_sha"
    SHOW_REF_SHA_TAG2="$commit_sha2"
    printf '%s\n' "$tag_obj_sha" "$commit_sha2"
}

setup_out_of_order_annotated_tag_refs() {
    init_repo
    SHOW_REF_SHA_ZEBRA=$(make_commit "zebra commit")
    SHOW_REF_SHA_ALPHA=$(make_commit "alpha commit")
    setup_git_identity
    git_tag -a z-release -m "z release" "$SHOW_REF_SHA_ZEBRA"
    git_tag -a a-release -m "a release" "$SHOW_REF_SHA_ALPHA"
    SHOW_REF_SHA_TAG_Z=$(git_rev_parse z-release)
    SHOW_REF_SHA_TAG_A=$(git_rev_parse a-release)
}
