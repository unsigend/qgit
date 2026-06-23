REV_PARSE_BRANCH="${QGIT_DEFAULT_BRANCH}"

init_repo() {
    "$QGIT_BIN" init -q
}

run_rev_parse() {
    run "$QGIT_BIN" rev-parse "$@"
}

git_rev_parse() {
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" rev-parse "$@"
}

make_commit() {
    local msg="${1:-test commit}"
    local blob_file="$TEST_DIR/rev-parse-blob-$$.txt"
    local commit_file="$TEST_DIR/rev-parse-commit-$$.txt"
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

setup_branch_head() {
    local sha

    init_repo
    sha=$(make_commit "$1")
    write_ref "refs/heads/$REV_PARSE_BRANCH" "$sha"
    set_symref_head "$REV_PARSE_BRANCH"
    printf '%s\n' "$sha"
}

assert_matches_git_rev_parse() {
    local expected

    expected=$(git_rev_parse "$@")
    run_rev_parse "$@"
    assert_success
    assert_output_equals "$expected"
}

abbrev_sha() {
    local sha="$1"
    local len="$2"

    printf '%s' "${sha:0:$len}"
}

uppercase_hex() {
    printf '%s' "$1" | tr '[:lower:]' '[:upper:]'
}

assert_output_is_full_sha() {
    assert_output_matches '^[0-9a-f]{40}$'
}

# Copy a loose object to a second name sharing the first prefix_len hex digits.
make_ambiguous_short_prefix() {
    local git_dir="$1"
    local sha="$2"
    local prefix_len="${3:-7}"
    local dir rest src dst fake_rest pad_len

    dir="${sha:0:2}"
    rest="${sha:2}"
    src="$git_dir/objects/$dir/$rest"
    [ -f "$src" ] || return 1

    pad_len=$((38 - prefix_len + 2))
    fake_rest="${rest:0:$((prefix_len - 2))}$(printf '%0*d' "$pad_len" 1)"
    if [ "$fake_rest" = "$rest" ]; then
        fake_rest="${rest:0:$((prefix_len - 2))}$(printf '%0*d' "$pad_len" 2)"
    fi

    dst="$git_dir/objects/$dir/$fake_rest"
    cp "$src" "$dst"
    printf '%s\n' "${sha:0:$prefix_len}"
}

setup_git_identity() {
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" config user.name "Test User"
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" config user.email "test@example.com"
}

git_tag() {
    env GIT_DIR="$(qgit_meta_dir)" "$GIT" tag "$@"
}

setup_annotated_tag_ref() {
    local tagname="${1:-v1.0}"
    local message="${2:-Release version 1.0}"
    local commit_sha tag_obj_sha

    init_repo
    commit_sha=$(make_commit "annotated tag commit")
    setup_git_identity
    git_tag -a "$tagname" -m "$message" "$commit_sha"
    tag_obj_sha=$(git_rev_parse "$tagname")
    REV_PARSE_ANNOTATED_COMMIT="$commit_sha"
    REV_PARSE_ANNOTATED_TAG="$tag_obj_sha"
    printf '%s\n' "$commit_sha" "$tag_obj_sha"
}

assert_matches_git_annotated_tag_rev_parse() {
    setup_annotated_tag_ref "$1" "${2:-Release version 1.0}" >/dev/null
    assert_matches_git_rev_parse "$1"
    assert_output_not_equals "$REV_PARSE_ANNOTATED_COMMIT"
}

setup_lightweight_tag_ref() {
    local tagname="$1"
    local sha="$2"

    write_ref "refs/tags/$tagname" "$sha"
}

setup_annotated_peel_fixture() {
    local tagname="${1:-v1.0}"

    setup_annotated_tag_ref "$tagname" "${2:-Release version 1.0}" >/dev/null
    REV_PARSE_PEEL_COMMIT="$REV_PARSE_ANNOTATED_COMMIT"
    REV_PARSE_PEEL_TAG="$REV_PARSE_ANNOTATED_TAG"
    REV_PARSE_PEEL_TREE=$(git_rev_parse "$REV_PARSE_PEEL_COMMIT^{tree}")
}

setup_nested_annotated_tags() {
    local commit_sha inner_tag outer_tag

    init_repo
    commit_sha=$(make_commit "nested tag commit")
    setup_git_identity
    git_tag -a inner -m "inner tag" "$commit_sha"
    inner_tag=$(git_rev_parse inner)
    git_tag -a outer -m "outer tag" inner
    outer_tag=$(git_rev_parse outer)
    REV_PARSE_NESTED_COMMIT="$commit_sha"
    REV_PARSE_NESTED_INNER="$inner_tag"
    REV_PARSE_NESTED_OUTER="$outer_tag"
}
