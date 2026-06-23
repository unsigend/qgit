load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "helpers/tag.bash"

# create

@test "qgit tag -a: create annotated tag at HEAD" {
    local sha

    sha=$(setup_branch_head "annotated head")
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" v1.0.0
    assert_success
    assert_output_empty
    assert_annotated_tag_points_at_commit "v1.0.0" "$sha"
    assert_tag_message_in_object "v1.0.0" "$TAG_ANNOTATED_MESSAGE"
}

@test "qgit tag -a: create annotated tag at explicit commit" {
    local sha other

    sha=$(setup_branch_head "annotated explicit base")
    other=$(make_commit "annotated explicit target")
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" release "$other"
    assert_success
    assert_annotated_tag_points_at_commit "release" "$other"
}

@test "qgit tag --annotate: create annotated tag with long flag" {
    local sha

    sha=$(setup_branch_head "annotated long flag")
    setup_user_identity
    run_tag --annotate -m "$TAG_ANNOTATED_MESSAGE" v1.0.0
    assert_success
    assert_annotated_tag_points_at_commit "v1.0.0" "$sha"
}

@test "qgit tag --message: create annotated tag with long message flag" {
    local sha

    sha=$(setup_branch_head "annotated long message flag")
    setup_user_identity
    run_tag -a --message "$TAG_ANNOTATED_MESSAGE" v1.0.0
    assert_success
    assert_annotated_tag_points_at_commit "v1.0.0" "$sha"
}

@test "qgit tag -a: multiline message is stored in tag object" {
    local sha message=$'First line\n\nThird line'

    sha=$(setup_branch_head "annotated multiline")
    setup_user_identity
    run_tag -a -m "$message" v2.0.0
    assert_success
    assert_annotated_tag_points_at_commit "v2.0.0" "$sha"
    assert_tag_message_in_object "v2.0.0" "First line"
    assert_tag_message_in_object "v2.0.0" "Third line"
}

@test "qgit tag -a: empty message creates annotated tag" {
    local sha

    sha=$(setup_branch_head "annotated empty message")
    setup_user_identity
    run_tag -a -m "" v1.0.0
    assert_success
    assert_output_empty
    assert_annotated_tag_points_at_commit "v1.0.0" "$sha"
}

@test "qgit tag -a: annotated tag ref points at tag object not commit" {
    local sha tag_ref_sha

    sha=$(setup_branch_head "annotated ref target")
    setup_qgit_annotated_tag v1.0.0 "$sha"
    tag_ref_sha=$(read_tag_ref "v1.0.0")
    assert_tag_ref_not_equals "v1.0.0" "$sha"
    assert_tag_object_type "$tag_ref_sha"
}

# object types

@test "qgit tag -a: create annotated tag pointing at tree object" {
    local tree_sha

    init_repo
    tree_sha=$(make_tree)
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" tree-release "$tree_sha"
    assert_success
    assert_output_empty
    assert_annotated_tag_points_at_object "tree-release" "$tree_sha" "tree"
    assert_tag_message_in_object "tree-release" "$TAG_ANNOTATED_MESSAGE"
}

@test "qgit tag -a: create annotated tag pointing at blob object" {
    local blob_sha

    init_repo
    blob_sha=$(make_blob)
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" blob-release "$blob_sha"
    assert_success
    assert_output_empty
    assert_annotated_tag_points_at_object "blob-release" "$blob_sha" "blob"
    assert_tag_message_in_object "blob-release" "$TAG_ANNOTATED_MESSAGE"
}

@test "qgit tag -a: create annotated tag pointing at tag object" {
    local commit_sha base_tag_sha

    init_repo
    commit_sha=$(make_commit "tag-to-tag base commit")
    base_tag_sha=$(make_annotated_tag_object "$commit_sha" "commit" "base-tag")
    setup_user_identity
    run_tag -a -m "tag on tag" on-tag "$base_tag_sha"
    assert_success
    assert_output_empty
    assert_annotated_tag_points_at_object "on-tag" "$base_tag_sha" "tag"
    assert_tag_message_in_object "on-tag" "tag on tag"
}

@test "qgit tag -a: create nested tag-to-tag chain" {
    local commit_sha tag1_sha tag2_sha

    init_repo
    commit_sha=$(make_commit "chain base commit")
    tag1_sha=$(make_annotated_tag_object "$commit_sha" "commit" "level1")
    tag2_sha=$(make_annotated_tag_object "$tag1_sha" "tag" "level2")
    setup_user_identity
    run_tag -a -m "level 3 tag" level3 "$tag2_sha"
    assert_success
    assert_annotated_tag_points_at_object "level3" "$tag2_sha" "tag"
}

@test "qgit tag -a: create annotated tag at empty tree" {
    local empty_tree_sha

    init_repo
    empty_tree_sha=$(make_empty_tree)
    setup_user_identity
    run_tag -a -m "empty tree release" empty-tree "$empty_tree_sha"
    assert_success
    assert_annotated_tag_points_at_object "empty-tree" "$empty_tree_sha" "tree"
}

# ref resolution

@test "qgit tag -a: resolve tree target via lightweight tag ref" {
    local tree_sha

    init_repo
    tree_sha=$(make_tree)
    run_tag tree-ref "$tree_sha"
    assert_success
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" tree-via-ref tree-ref
    assert_success
    assert_annotated_tag_points_at_object "tree-via-ref" "$tree_sha" "tree"
}

@test "qgit tag -a: resolve blob target via lightweight tag ref" {
    local blob_sha

    init_repo
    blob_sha=$(make_blob)
    run_tag blob-ref "$blob_sha"
    assert_success
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" blob-via-ref blob-ref
    assert_success
    assert_annotated_tag_points_at_object "blob-via-ref" "$blob_sha" "blob"
}

@test "qgit tag -a: resolve tag object target via lightweight tag ref" {
    local commit_sha base_tag_sha

    init_repo
    commit_sha=$(make_commit "ref to tag base")
    base_tag_sha=$(make_annotated_tag_object "$commit_sha" "commit" "base-tag")
    run_tag tag-ref "$base_tag_sha"
    assert_success
    setup_user_identity
    run_tag -a -m "via ref to tag" tag-via-ref tag-ref
    assert_success
    assert_annotated_tag_points_at_object "tag-via-ref" "$base_tag_sha" "tag"
}

@test "qgit tag -a: resolve annotated tag target via existing annotated tag ref" {
    local commit_sha

    init_repo
    commit_sha=$(make_commit "annotated ref base")
    setup_user_identity
    run_tag -a -m "first annotated" first-tag "$commit_sha"
    assert_success
    run_tag -a -m "second annotated" second-tag first-tag
    assert_success
    assert_annotated_tag_points_at_object "second-tag" "$(read_tag_ref first-tag)" "tag"
}

# git parity

@test "qgit tag -a: tree target matches git annotated tag" {
    local tree_sha

    init_repo
    tree_sha=$(make_tree)
    assert_matches_git_annotated_tag_on_object tree-release "$tree_sha" "tree"
}

@test "qgit tag -a: blob target matches git annotated tag" {
    local blob_sha

    init_repo
    blob_sha=$(make_blob)
    assert_matches_git_annotated_tag_on_object blob-release "$blob_sha" "blob"
}

@test "qgit tag -a: tag object target matches git annotated tag" {
    local commit_sha base_tag_sha

    init_repo
    commit_sha=$(make_commit "git tag-to-tag base")
    base_tag_sha=$(make_annotated_tag_object "$commit_sha" "commit" "base-tag")
    assert_matches_git_annotated_tag_on_object on-tag "$base_tag_sha" "tag"
}

# -f

@test "qgit tag -f: force overwrite annotated tag" {
    local sha second

    sha=$(setup_branch_head "annotated force first")
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" stable "$sha"
    assert_success
    second=$(make_commit "annotated force second")
    run_tag -f -a -m "forced release" stable "$second"
    assert_success
    assert_annotated_tag_points_at_commit "stable" "$second"
    assert_tag_message_in_object "stable" "forced release"
}

@test "qgit tag -a: force overwrite annotated tag to different object type" {
    local commit_sha tree_sha

    init_repo
    commit_sha=$(make_commit "force type switch commit")
    tree_sha=$(make_tree)
    setup_user_identity
    run_tag -a -m "commit tag" switch-tag "$commit_sha"
    assert_success
    assert_annotated_tag_points_at_object "switch-tag" "$commit_sha" "commit"
    run_tag -f -a -m "tree tag" switch-tag "$tree_sha"
    assert_success
    assert_annotated_tag_points_at_object "switch-tag" "$tree_sha" "tree"
    assert_tag_message_in_object "switch-tag" "tree tag"
}

# -d

@test "qgit tag -d: delete annotated tag" {
    local sha

    sha=$(setup_branch_head "annotated delete")
    setup_qgit_annotated_tag v1.0.0 "$sha"
    run_tag -d v1.0.0
    assert_success
    assert_output_empty
    [ ! -f "$(tag_ref_file v1.0.0)" ]
}

@test "qgit tag -d: delete annotated tag on tree object" {
    local tree_sha

    init_repo
    tree_sha=$(make_tree)
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" tree-delete "$tree_sha"
    assert_success
    run_tag -d tree-delete
    assert_success
    assert_output_empty
    [ ! -f "$(tag_ref_file tree-delete)" ]
}

@test "qgit tag -d: matches git delete for annotated tag" {
    local sha

    sha=$(setup_branch_head "git delete annotated")
    setup_git_identity
    git_tag -a alpha -m "$TAG_ANNOTATED_MESSAGE" "$sha"
    git_tag -a beta -m "$TAG_ANNOTATED_MESSAGE" "$sha"
    git_tag -d alpha
    run_tag -d beta
    assert_success
    assert_matches_git_tag_list
}

# list

@test "qgit tag: list includes annotated tag names only" {
    local sha

    sha=$(setup_branch_head "annotated list")
    setup_qgit_annotated_tag v1.0.0 "$sha"
    run_tag lightweight "$sha"
    assert_success
    run_tag
    assert_success
    assert_output_contains "v1.0.0"
    assert_output_contains "lightweight"
    assert_output_not_contains "refs/tags/"
}

@test "qgit tag: list mixed lightweight and annotated tags matches git" {
    local sha

    sha=$(setup_branch_head "annotated git list mix")
    setup_git_identity
    git_tag -a alpha -m "$TAG_ANNOTATED_MESSAGE" "$sha"
    run_tag beta "$sha"
    assert_success
    setup_user_identity
    run_tag -a -m "beta annotated" gamma "$sha"
    assert_success
    assert_matches_git_tag_list_ordered
}

@test "qgit tag: list annotated tags in lexicographic order matches git" {
    local sha

    sha=$(setup_branch_head "annotated list order")
    setup_git_identity
    git_tag -a v1.3.0 -m "$TAG_ANNOTATED_MESSAGE" "$sha"
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" v1.0.0 "$sha"
    assert_success
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" v1.1.0 "$sha"
    assert_success
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" v2.0.0 "$sha"
    assert_success
    assert_matches_git_tag_list_ordered
}

@test "qgit tag: list includes annotated tags on non-commit objects" {
    local tree_sha blob_sha

    init_repo
    tree_sha=$(make_tree)
    blob_sha=$(make_blob)
    setup_user_identity
    run_tag -a -m "tree list" tree-list "$tree_sha"
    assert_success
    run_tag -a -m "blob list" blob-list "$blob_sha"
    assert_success
    run_tag
    assert_success
    assert_output_contains "tree-list"
    assert_output_contains "blob-list"
}

# errors

@test "qgit tag -a: missing message fails" {
    setup_branch_head "annotated missing message"
    setup_user_identity
    run_tag -a v1.0.0
    assert_failure
    [ ! -f "$(tag_ref_file v1.0.0)" ]
}

@test "qgit tag -a: fails without user identity" {
    setup_branch_head "annotated missing identity"
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" v1.0.0
    assert_failure
    [ ! -f "$(tag_ref_file v1.0.0)" ]
}

@test "qgit tag -a: fails when HEAD does not resolve" {
    init_repo
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" v1.0.0
    assert_failure
}

@test "qgit tag -a: fails with unresolvable object" {
    init_repo
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" v1.0.0 deadbeefdeadbeefdeadbeefdeadbeefdeadbeef
    assert_failure
    [ ! -f "$(tag_ref_file v1.0.0)" ]
}

@test "qgit tag -a: duplicate annotated tag without force fails" {
    local sha second

    sha=$(setup_branch_head "annotated duplicate first")
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" stable "$sha"
    assert_success
    second=$(make_commit "annotated duplicate second")
    run_tag -a -m "other message" stable "$second"
    assert_failure
    assert_annotated_tag_points_at_commit "stable" "$sha"
}

@test "qgit tag -a: duplicate annotated tag on blob without force fails" {
    local blob_sha second_blob_sha

    init_repo
    blob_sha=$(make_blob "first blob")
    second_blob_sha=$(make_blob "second blob")
    setup_user_identity
    run_tag -a -m "$TAG_ANNOTATED_MESSAGE" stable-blob "$blob_sha"
    assert_success
    run_tag -a -m "other message" stable-blob "$second_blob_sha"
    assert_failure
    assert_annotated_tag_points_at_object "stable-blob" "$blob_sha" "blob"
}
