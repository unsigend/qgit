load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "helpers/show-ref.bash"

# show-ref

@test "qgit show-ref: outside repository fails" {
    run_show_ref
    assert_failure
}

@test "qgit show-ref: fresh repository produces no output" {
    init_repo
    run_show_ref
    assert_success
    assert_output_empty
}

@test "qgit show-ref: lists branch references by default" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    run_show_ref
    assert_success
    assert_output_equals "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
}

@test "qgit show-ref: lists tag references by default" {
    init_repo
    SHOW_REF_SHA_TAG=$(make_commit "tag commit")
    write_ref "refs/tags/v1.0.0" "$SHOW_REF_SHA_TAG"
    run_show_ref
    assert_success
    assert_output_equals "$(ref_line "$SHOW_REF_SHA_TAG" "refs/tags/v1.0.0")"
}

@test "qgit show-ref: lists branches and tags together" {
    setup_basic_refs
    run_show_ref
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_DEV" "refs/heads/dev")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_TAG" "refs/tags/v1.0.0")"
}

@test "qgit show-ref: default output omits HEAD" {
    setup_basic_refs
    set_symref_head "$SHOW_REF_BRANCH"
    run_show_ref
    assert_success
    assert_output_not_contains " HEAD"
    assert_output_not_contains "^HEAD "
}

@test "qgit show-ref: matches git output for loose refs" {
    setup_basic_refs
    assert_matches_git_show_ref
}

@test "qgit show-ref: works when tags directory is missing" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    rmdir "$(qgit_meta_dir)/refs/tags"
    run_show_ref
    assert_success
    assert_output_equals "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
}

# --head

@test "qgit show-ref --head: prints resolved HEAD for symref" {
    setup_basic_refs
    set_symref_head "$SHOW_REF_BRANCH"
    run_show_ref --head
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "HEAD")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
}

@test "qgit show-ref --head: prints detached HEAD directly" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    set_detached_head "$SHOW_REF_SHA_DETACHED"
    run_show_ref --head
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_DETACHED" "HEAD")"
    assert_output_not_contains "$(ref_line "$SHOW_REF_SHA_DETACHED" "refs/heads/$SHOW_REF_BRANCH")"
}

@test "qgit show-ref --head: matches git output" {
    setup_basic_refs
    set_symref_head "$SHOW_REF_BRANCH"
    assert_matches_git_show_ref --head
}

@test "qgit show-ref --head: fails when symref target is missing" {
    init_repo
    set_symref_head "$SHOW_REF_BRANCH"
    run_show_ref --head
    assert_failure
}

@test "qgit show-ref --head --branches: includes HEAD and branches only" {
    setup_basic_refs
    set_symref_head "$SHOW_REF_BRANCH"
    run_show_ref --head --branches
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "HEAD")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_DEV" "refs/heads/dev")"
    assert_output_not_contains "refs/tags/"
}

@test "qgit show-ref --head --tags: includes HEAD and tags only" {
    setup_basic_refs
    set_symref_head "$SHOW_REF_BRANCH"
    run_show_ref --head --tags
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "HEAD")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_TAG" "refs/tags/v1.0.0")"
    assert_output_not_contains "refs/heads/dev"
}

# --branches

@test "qgit show-ref --branches: lists branches only" {
    setup_basic_refs
    run_show_ref --branches
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_DEV" "refs/heads/dev")"
    assert_output_not_contains "refs/tags/"
}

@test "qgit show-ref --branches: matches git output" {
    setup_basic_refs
    assert_matches_git_show_ref --branches
}

@test "qgit show-ref --branches: empty when no branches exist" {
    init_repo
    SHOW_REF_SHA_TAG=$(make_commit "tag commit")
    write_ref "refs/tags/v1.0.0" "$SHOW_REF_SHA_TAG"
    run_show_ref --branches
    assert_success
    assert_output_empty
}

# --tags

@test "qgit show-ref --tags: lists tags only" {
    setup_basic_refs
    run_show_ref --tags
    assert_success
    assert_output_equals "$(ref_line "$SHOW_REF_SHA_TAG" "refs/tags/v1.0.0")"
    assert_output_not_contains "refs/heads/"
}

@test "qgit show-ref --tags: matches git output" {
    setup_basic_refs
    assert_matches_git_show_ref --tags
}

@test "qgit show-ref --tags: empty when no tags exist" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    run_show_ref --tags
    assert_success
    assert_output_empty
}

# --branches --tags

@test "qgit show-ref --branches --tags: lists both namespaces" {
    setup_basic_refs
    run_show_ref --branches --tags
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_DEV" "refs/heads/dev")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_TAG" "refs/tags/v1.0.0")"
}

@test "qgit show-ref --branches --tags: matches git output" {
    setup_basic_refs
    assert_matches_git_show_ref --branches --tags
}

@test "qgit show-ref --head --branches --tags: matches git output" {
    setup_basic_refs
    set_symref_head "$SHOW_REF_BRANCH"
    assert_matches_git_show_ref --head --branches --tags
}

# nested refs

@test "qgit show-ref: lists nested branch names" {
    init_repo
    SHOW_REF_SHA_NESTED=$(make_commit "nested branch commit")
    write_ref "refs/heads/feature/nested" "$SHOW_REF_SHA_NESTED"
    run_show_ref
    assert_success
    assert_output_equals "$(ref_line "$SHOW_REF_SHA_NESTED" "refs/heads/feature/nested")"
}

@test "qgit show-ref --branches: lists nested branch with other branches" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    SHOW_REF_SHA_NESTED=$(make_commit "nested branch commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    write_ref "refs/heads/feature/nested" "$SHOW_REF_SHA_NESTED"
    run_show_ref --branches
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "refs/heads/$SHOW_REF_BRANCH")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_NESTED" "refs/heads/feature/nested")"
}

@test "qgit show-ref: nested branch matches git output" {
    init_repo
    SHOW_REF_SHA_NESTED=$(make_commit "nested branch commit")
    write_ref "refs/heads/feature/nested" "$SHOW_REF_SHA_NESTED"
    assert_matches_git_show_ref
}

@test "qgit show-ref --tags: lists nested tag names" {
    setup_multi_tag_refs
    run_show_ref --tags
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_TAG" "refs/tags/v1.0.0")"
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_TAG2" "refs/tags/release/v2.0.0")"
    assert_output_not_contains "refs/heads/"
}

@test "qgit show-ref: multiple tags match git output" {
    setup_multi_tag_refs
    assert_matches_git_show_ref
}

# output order

@test "qgit show-ref: output order matches git for mixed branches and tags" {
    setup_out_of_order_refs
    assert_matches_git_show_ref_order
}

@test "qgit show-ref --branches: output order matches git" {
    setup_out_of_order_refs
    assert_matches_git_show_ref_order --branches
}

@test "qgit show-ref --tags: output order matches git" {
    setup_out_of_order_refs
    assert_matches_git_show_ref_order --tags
}

@test "qgit show-ref --branches --tags: output order matches git" {
    setup_out_of_order_refs
    assert_matches_git_show_ref_order --branches --tags
}

@test "qgit show-ref --head: output order matches git" {
    setup_out_of_order_refs
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_ALPHA"
    set_symref_head "$SHOW_REF_BRANCH"
    assert_matches_git_show_ref_order --head
}

@test "qgit show-ref --head --branches: output order matches git" {
    setup_out_of_order_refs
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_ALPHA"
    set_symref_head "$SHOW_REF_BRANCH"
    assert_matches_git_show_ref_order --head --branches
}

@test "qgit show-ref --head --tags: output order matches git" {
    setup_out_of_order_refs
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_ALPHA"
    set_symref_head "$SHOW_REF_BRANCH"
    assert_matches_git_show_ref_order --head --tags
}

@test "qgit show-ref: nested branch output order matches git" {
    setup_nested_out_of_order_refs
    assert_matches_git_show_ref_order --branches
}

@test "qgit show-ref: nested tag output order matches git" {
    setup_multi_tag_refs
    assert_matches_git_show_ref_order --tags
}

# output format

@test "qgit show-ref: prints oid and refname separated by space" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    run_show_ref
    assert_success
    assert_output_matches "^[0-9a-f]{40} refs/heads/${SHOW_REF_BRANCH}$"
}

@test "qgit show-ref --head: HEAD line uses lowercase oid" {
    setup_basic_refs
    set_symref_head "$SHOW_REF_BRANCH"
    run_show_ref --head
    assert_success
    assert_output_contains "$(ref_line "$SHOW_REF_SHA_MAIN" "HEAD")"
}

# -h

@test "qgit show-ref -h: shows help" {
    run_show_ref -h
    assert_success
    assert_output_contains "qgit show-ref"
    assert_output_contains "--head"
    assert_output_contains "--branches"
    assert_output_contains "--tags"
}

@test "qgit show-ref --help: shows help" {
    run_show_ref --help
    assert_success
    assert_output_contains "qgit show-ref"
    assert_output_contains "List references in a local repository"
}

# errors

@test "qgit show-ref: unknown flag fails" {
    init_repo
    run_show_ref --unknown-flag
    assert_failure
}

@test "qgit show-ref: invalid ref file fails" {
    init_repo
    write_ref "refs/heads/broken" "not-a-valid-sha"
    run_show_ref --branches
    assert_failure
}

@test "qgit show-ref: invalid tag ref fails on default listing" {
    init_repo
    SHOW_REF_SHA_MAIN=$(make_commit "main commit")
    write_ref "refs/heads/$SHOW_REF_BRANCH" "$SHOW_REF_SHA_MAIN"
    write_ref "refs/tags/broken" "not-a-valid-sha"
    run_show_ref
    assert_failure
}
