load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "helpers/cat-file.bash"
load "helpers/cat-file-commit.bash"
load "helpers/cat-file-tree.bash"
load "helpers/cat-file-tag.bash"

# tag -p

@test "qgit cat-file -p: annotated commit tag matches git" {
    local hash
    hash=$(setup_annotated_commit_tag)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: annotated tag with empty message matches git" {
    local hash
    hash=$(setup_annotated_commit_tag_empty_message)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: annotated tag with multiline message matches git" {
    local hash
    hash=$(setup_annotated_commit_tag_multiline_message)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: annotated tag on tree object matches git" {
    local hash
    hash=$(setup_annotated_tree_tag)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: annotated tag on blob object matches git" {
    local hash
    hash=$(setup_annotated_blob_tag)
    assert_matches_git_cat_p "$hash"
}

# tag parse errors

@test "qgit cat-file -p: tag missing object line fails" {
    local file="$TEST_DIR/tag-missing-object.txt"

    write_tag_missing_object "$file"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag missing type line fails" {
    local file="$TEST_DIR/tag-missing-type.txt"
    local commit_sha

    commit_sha=$(setup_root_commit)
    write_tag_missing_type "$file" "$commit_sha"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag missing tag name line fails" {
    local file="$TEST_DIR/tag-missing-name.txt"
    local commit_sha

    commit_sha=$(setup_root_commit)
    write_tag_missing_name "$file" "$commit_sha"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag missing tagger line fails" {
    local file="$TEST_DIR/tag-missing-tagger.txt"
    local commit_sha

    commit_sha=$(setup_root_commit)
    write_tag_missing_tagger "$file" "$commit_sha"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag missing all header lines fails" {
    local file="$TEST_DIR/tag-missing-headers.txt"

    write_tag_missing_all_headers "$file"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag duplicate object line fails" {
    local file="$TEST_DIR/tag-duplicate-object.txt"
    local commit_sha

    commit_sha=$(setup_root_commit)
    write_tag_duplicate_object "$file" "$commit_sha"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag duplicate type line fails" {
    local file="$TEST_DIR/tag-duplicate-type.txt"
    local commit_sha

    commit_sha=$(setup_root_commit)
    write_tag_duplicate_type "$file" "$commit_sha"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag duplicate tag name line fails" {
    local file="$TEST_DIR/tag-duplicate-name.txt"
    local commit_sha

    commit_sha=$(setup_root_commit)
    write_tag_duplicate_name "$file" "$commit_sha"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag duplicate tagger line fails" {
    local file="$TEST_DIR/tag-duplicate-tagger.txt"
    local commit_sha

    commit_sha=$(setup_root_commit)
    write_tag_duplicate_tagger "$file" "$commit_sha"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag unknown header line fails" {
    local file="$TEST_DIR/tag-unknown-header.txt"
    local commit_sha

    commit_sha=$(setup_root_commit)
    write_tag_unknown_header "$file" "$commit_sha"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag invalid object sha fails" {
    local file="$TEST_DIR/tag-invalid-object-sha.txt"

    write_tag_invalid_object_sha "$file"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag truncated object sha fails" {
    local file="$TEST_DIR/tag-truncated-object-sha.txt"

    write_tag_truncated_object_sha "$file"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag invalid type fails" {
    local file="$TEST_DIR/tag-invalid-type.txt"
    local commit_sha

    commit_sha=$(setup_root_commit)
    write_tag_invalid_type "$file" "$commit_sha"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

@test "qgit cat-file -p: tag bad tagger signature fails" {
    local file="$TEST_DIR/tag-bad-tagger.txt"
    local commit_sha

    commit_sha=$(setup_root_commit)
    write_tag_bad_tagger_signature "$file" "$commit_sha"
    assert_cat_file_p_rejects_malformed_tag "$file"
}

# tag -t

@test "qgit cat-file -t: annotated commit tag type matches git" {
    local hash
    hash=$(setup_annotated_commit_tag)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: annotated tag on tree type matches git" {
    local hash
    hash=$(setup_annotated_tree_tag)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: annotated tag on blob type matches git" {
    local hash
    hash=$(setup_annotated_blob_tag)
    assert_matches_git_cat -t "$hash"
}

# tag -s

@test "qgit cat-file -s: annotated commit tag size matches git" {
    local hash
    hash=$(setup_annotated_commit_tag)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: annotated tag with empty message size matches git" {
    local hash
    hash=$(setup_annotated_commit_tag_empty_message)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: annotated tag with multiline message size matches git" {
    local hash
    hash=$(setup_annotated_commit_tag_multiline_message)
    assert_matches_git_cat -s "$hash"
}

# tag raw

@test "qgit cat-file tag: annotated commit tag matches git" {
    local hash
    hash=$(setup_annotated_commit_tag)
    assert_matches_git_cat tag "$hash"
}

@test "qgit cat-file tag: annotated tag on tree matches git" {
    local hash
    hash=$(setup_annotated_tree_tag)
    assert_matches_git_cat tag "$hash"
}

@test "qgit cat-file tag: blob type on tag object fails" {
    local hash
    hash=$(setup_annotated_commit_tag)
    run_cat_file blob "$hash"
    assert_failure
}

@test "qgit cat-file tag: commit type on tag object fails" {
    local hash
    hash=$(setup_annotated_commit_tag)
    run_cat_file commit "$hash"
    assert_failure
}

@test "qgit cat-file tag: tree type on tag object fails" {
    local hash
    hash=$(setup_annotated_commit_tag)
    run_cat_file tree "$hash"
    assert_failure
}

# tag errors

@test "qgit cat-file -p: missing tag object file fails" {
    setup_annotated_commit_tag >/dev/null
    run_cat_file -p 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -t: missing tag object file fails" {
    setup_annotated_commit_tag >/dev/null
    run_cat_file -t 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -s: missing tag object file fails" {
    setup_annotated_commit_tag >/dev/null
    run_cat_file -s 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file tag: missing tag object file fails" {
    setup_annotated_commit_tag >/dev/null
    run_cat_file tag 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -p -t: flags cannot combine on tag" {
    local hash
    hash=$(setup_annotated_commit_tag)
    run_cat_file -p -t "$hash"
    assert_failure
}

@test "qgit cat-file -p -s: flags cannot combine on tag" {
    local hash
    hash=$(setup_annotated_commit_tag)
    run_cat_file -p -s "$hash"
    assert_failure
}

@test "qgit cat-file -s -t: flags cannot combine on tag" {
    local hash
    hash=$(setup_annotated_commit_tag)
    run_cat_file -s -t "$hash"
    assert_failure
}
