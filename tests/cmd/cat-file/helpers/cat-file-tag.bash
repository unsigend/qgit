store_malformed_tag_payload() {
    local file="$1"
    local sha type

    init_repo
    sha=$("$QGIT_BIN" hash-object -w -t tag "$file")
    if [ -z "$sha" ]; then
        echo "qgit hash-object failed to store malformed tag" >&2
        return 1
    fi

    type=$("$QGIT_BIN" cat-file -t "$sha")
    if [ "$type" != "tag" ]; then
        echo "expected stored tag object, got type: $type" >&2
        return 1
    fi

    printf '%s\n' "$sha"
}

assert_cat_file_p_rejects_malformed_tag() {
    local file="$1"
    local hash

    hash=$(store_malformed_tag_payload "$file")
    run_cat_file -p "$hash"
    assert_failure
}

write_tag_missing_object() {
    local file="$1"

    cat >"$file" <<EOF
type commit
tag v1.0
tagger Test User <test@example.com> 946684800 +0000

missing object line
EOF
}

write_tag_missing_type() {
    local file="$1"
    local commit_sha="$2"

    cat >"$file" <<EOF
object $commit_sha
tag v1.0
tagger Test User <test@example.com> 946684800 +0000

missing type line
EOF
}

write_tag_missing_name() {
    local file="$1"
    local commit_sha="$2"

    cat >"$file" <<EOF
object $commit_sha
type commit
tagger Test User <test@example.com> 946684800 +0000

missing tag name line
EOF
}

write_tag_missing_tagger() {
    local file="$1"
    local commit_sha="$2"

    cat >"$file" <<EOF
object $commit_sha
type commit
tag v1.0

missing tagger line
EOF
}

write_tag_missing_all_headers() {
    local file="$1"

    cat >"$file" <<EOF

missing all header lines
EOF
}

write_tag_duplicate_object() {
    local file="$1"
    local commit_sha="$2"

    cat >"$file" <<EOF
object $commit_sha
object $commit_sha
type commit
tag v1.0
tagger Test User <test@example.com> 946684800 +0000

duplicate object line
EOF
}

write_tag_duplicate_type() {
    local file="$1"
    local commit_sha="$2"

    cat >"$file" <<EOF
object $commit_sha
type commit
type commit
tag v1.0
tagger Test User <test@example.com> 946684800 +0000

duplicate type line
EOF
}

write_tag_duplicate_name() {
    local file="$1"
    local commit_sha="$2"

    cat >"$file" <<EOF
object $commit_sha
type commit
tag v1.0
tag v1.0
tagger Test User <test@example.com> 946684800 +0000

duplicate tag name line
EOF
}

write_tag_duplicate_tagger() {
    local file="$1"
    local commit_sha="$2"

    cat >"$file" <<EOF
object $commit_sha
type commit
tag v1.0
tagger Test User <test@example.com> 946684800 +0000
tagger Test User <test@example.com> 946684800 +0000

duplicate tagger line
EOF
}

write_tag_unknown_header() {
    local file="$1"
    local commit_sha="$2"

    cat >"$file" <<EOF
object $commit_sha
type commit
unknown header line
tag v1.0
tagger Test User <test@example.com> 946684800 +0000

unknown header line
EOF
}

write_tag_invalid_object_sha() {
    local file="$1"

    cat >"$file" <<EOF
object ggggggggggggggggggggggggggggggggggggggg
type commit
tag v1.0
tagger Test User <test@example.com> 946684800 +0000

invalid object sha
EOF
}

write_tag_truncated_object_sha() {
    local file="$1"

    cat >"$file" <<EOF
object 000000000000000000000000000000000000000
type commit
tag v1.0
tagger Test User <test@example.com> 946684800 +0000

truncated object sha
EOF
}

write_tag_invalid_type() {
    local file="$1"
    local commit_sha="$2"

    cat >"$file" <<EOF
object $commit_sha
type not-a-valid-type
tag v1.0
tagger Test User <test@example.com> 946684800 +0000

invalid type
EOF
}

write_tag_bad_tagger_signature() {
    local file="$1"
    local commit_sha="$2"

    cat >"$file" <<EOF
object $commit_sha
type commit
tag v1.0
tagger bad signature line

bad tagger signature
EOF
}

setup_annotated_commit_tag() {
    local commit_sha tag_sha

    commit_sha=$(setup_root_commit)
    env GIT_DIR="$TEST_DIR/.git" "$GIT" tag -a v1.0 -m "Release version 1.0" "$commit_sha"
    tag_sha=$(env GIT_DIR="$TEST_DIR/.git" "$GIT" rev-parse v1.0)
    sync_git_to_qgit
    echo "$tag_sha"
}

setup_annotated_commit_tag_empty_message() {
    local commit_sha tag_sha file="$TEST_DIR/tag-empty-message.txt"

    commit_sha=$(setup_root_commit)
    cat >"$file" <<EOF
object $commit_sha
type commit
tag empty-msg
tagger Test User <test@example.com> 946684800 +0000

EOF
    tag_sha=$(git_write_tag "$file")
    sync_git_to_qgit
    echo "$tag_sha"
}

setup_annotated_commit_tag_multiline_message() {
    local commit_sha tag_sha

    commit_sha=$(setup_root_commit)
    env GIT_DIR="$TEST_DIR/.git" "$GIT" tag -a v2.0 -m $'First line\n\nThird line' "$commit_sha"
    tag_sha=$(env GIT_DIR="$TEST_DIR/.git" "$GIT" rev-parse v2.0)
    sync_git_to_qgit
    echo "$tag_sha"
}

setup_annotated_tree_tag() {
    local tree_sha tag_sha file="$TEST_DIR/tag-on-tree.txt"

    tree_sha=$(setup_single_file_tree)
    cat >"$file" <<EOF
object $tree_sha
type tree
tag tree-release
tagger Test User <test@example.com> 946684800 +0000

Tag pointing at tree object
EOF
    tag_sha=$(git_write_tag "$file")
    sync_git_to_qgit
    echo "$tag_sha"
}

setup_annotated_blob_tag() {
    local blob_sha tag_sha file="$TEST_DIR/tag-on-blob.txt"

    init_mock_git
    printf 'tagged blob content\n' >blob-tag.txt
    blob_sha=$(git_write_blob blob-tag.txt)
    cat >"$file" <<EOF
object $blob_sha
type blob
tag blob-release
tagger Test User <test@example.com> 946684800 +0000

Tag pointing at blob object
EOF
    tag_sha=$(git_write_tag "$file")
    sync_git_to_qgit
    echo "$tag_sha"
}
