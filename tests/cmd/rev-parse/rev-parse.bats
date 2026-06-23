load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "helpers/rev-parse.bash"

# rev-parse

@test "qgit rev-parse: no arguments succeeds with no output" {
    setup_branch_head
    run_rev_parse
    assert_success
    assert_output_empty
}

@test "qgit rev-parse: no arguments succeeds outside repository" {
    run_rev_parse
    assert_success
    assert_output_empty
}

# full sha1

@test "qgit rev-parse: full sha1 resolves to itself" {
    local sha

    sha=$(setup_branch_head)
    assert_matches_git_rev_parse "$sha"
}

@test "qgit rev-parse: full sha1 with uppercase hex resolves to lowercase" {
    local sha upper

    sha=$(setup_branch_head)
    upper=$(uppercase_hex "$sha")
    run_rev_parse "$upper"
    assert_success
    assert_output_equals "$sha"
}

@test "qgit rev-parse: full sha1 with mixed case resolves to lowercase" {
    local sha mixed

    sha=$(setup_branch_head)
    mixed="${sha:0:8}$(uppercase_hex "${sha:8:8}")${sha:16}"
    run_rev_parse "$mixed"
    assert_success
    assert_output_equals "$sha"
}

@test "qgit rev-parse: invalid full sha1 fails" {
    setup_branch_head
    run_rev_parse "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"
    assert_failure
}

@test "qgit rev-parse: full sha1 with one invalid hex character fails" {
    setup_branch_head
    run_rev_parse "000000000000000000000000000000000000000g"
    assert_failure
}

@test "qgit rev-parse: full sha1 with no object fails" {
    setup_branch_head >/dev/null
    run_rev_parse "0000000000000000000000000000000000000001"
    assert_failure
}

@test "qgit rev-parse: 39 character hex is not treated as full sha1" {
    setup_branch_head
    run_rev_parse "000000000000000000000000000000000000001"
    assert_failure
}

@test "qgit rev-parse: 41 character hex fails" {
    setup_branch_head
    run_rev_parse "00000000000000000000000000000000000000001"
    assert_failure
}

@test "qgit rev-parse: output is always 40 lowercase hex digits" {
    local sha

    sha=$(setup_branch_head)
    run_rev_parse "$sha"
    assert_success
    assert_output_is_full_sha
}

# HEAD

@test "qgit rev-parse: HEAD resolves through symref" {
    local sha

    sha=$(setup_branch_head)
    assert_matches_git_rev_parse HEAD
    assert_output_equals "$sha"
}

@test "qgit rev-parse: HEAD resolves when detached" {
    local sha

    sha=$(setup_branch_head)
    set_detached_head "$sha"
    assert_matches_git_rev_parse HEAD
}

@test "qgit rev-parse: HEAD resolves through symref to nested branch" {
    local sha

    init_repo
    sha=$(make_commit)
    write_ref "refs/heads/feature/foo" "$sha"
    set_symref_head "feature/foo"
    assert_matches_git_rev_parse HEAD
}

@test "qgit rev-parse: HEAD fails when branch ref is missing" {
    init_repo
    run_rev_parse HEAD
    assert_failure
    run env GIT_DIR="$(qgit_meta_dir)" "$GIT" rev-parse HEAD
    assert_failure
}

@test "qgit rev-parse: HEAD fails when symref target is missing" {
    init_repo
    set_symref_head "$REV_PARSE_BRANCH"
    run_rev_parse HEAD
    assert_failure
}

@test "qgit rev-parse: HEAD fails with invalid detached sha" {
    init_repo
    set_detached_head "not-a-valid-sha1-hash-value-here000000"
    run_rev_parse HEAD
    assert_failure
}

@test "qgit rev-parse: lowercase head is not treated as HEAD" {
    setup_branch_head
    run_rev_parse head
    assert_failure
}

# refs path

@test "qgit rev-parse: refs/heads path resolves branch" {
    local sha

    sha=$(setup_branch_head)
    assert_matches_git_rev_parse "refs/heads/$REV_PARSE_BRANCH"
}

@test "qgit rev-parse: refs/tags path resolves tag" {
    local sha

    init_repo
    sha=$(make_commit)
    write_ref "refs/tags/v1.0" "$sha"
    assert_matches_git_rev_parse refs/tags/v1.0
}

@test "qgit rev-parse: missing refs/heads path fails" {
    setup_branch_head
    run_rev_parse refs/heads/no-such-branch
    assert_failure
}

@test "qgit rev-parse: missing refs/tags path fails" {
    setup_branch_head
    run_rev_parse refs/tags/no-such-tag
    assert_failure
}

@test "qgit rev-parse: nested refs/heads path resolves branch" {
    local sha

    init_repo
    sha=$(make_commit)
    write_ref "refs/heads/feature/foo" "$sha"
    assert_matches_git_rev_parse refs/heads/feature/foo
}

@test "qgit rev-parse: refs path matches bare branch name output" {
    local sha

    sha=$(setup_branch_head)
    run_rev_parse "refs/heads/$REV_PARSE_BRANCH"
    assert_success
    assert_output_equals "$sha"
    run_rev_parse "$REV_PARSE_BRANCH"
    assert_success
    assert_output_equals "$sha"
}

# branch name

@test "qgit rev-parse: branch name resolves active branch" {
    local sha

    sha=$(setup_branch_head)
    assert_matches_git_rev_parse "$REV_PARSE_BRANCH"
}

@test "qgit rev-parse: non-default branch name resolves by bare name" {
    local sha

    init_repo
    sha=$(make_commit)
    write_ref "refs/heads/dev" "$sha"
    assert_matches_git_rev_parse dev
}

@test "qgit rev-parse: nested branch name resolves by bare name" {
    local sha

    init_repo
    sha=$(make_commit)
    write_ref "refs/heads/feature/foo" "$sha"
    assert_matches_git_rev_parse feature/foo
}

@test "qgit rev-parse: branch name wins over short sha1 prefix" {
    local sha branch_name

    sha=$(setup_branch_head)
    branch_name=$(abbrev_sha "$sha" 7)
    write_ref "refs/heads/$branch_name" "$sha"
    run_rev_parse "$branch_name"
    assert_success
    assert_output_equals "$sha"
}

@test "qgit rev-parse: branch name wins over short sha1 when name is 39 hex digits" {
    local sha branch_name

    sha=$(setup_branch_head)
    branch_name=$(abbrev_sha "$sha" 39)
    write_ref "refs/heads/$branch_name" "$sha"
    run_rev_parse "$branch_name"
    assert_success
    assert_output_equals "$sha"
}

# tag name

@test "qgit rev-parse: tag name resolves without matching branch" {
    local sha

    init_repo
    sha=$(make_commit)
    write_ref "refs/tags/only-tag" "$sha"
    assert_matches_git_rev_parse only-tag
}

@test "qgit rev-parse: nested tag name resolves by bare name" {
    local sha

    init_repo
    sha=$(make_commit)
    write_ref "refs/tags/release/v1.0" "$sha"
    assert_matches_git_rev_parse release/v1.0
}

@test "qgit rev-parse: tag resolves when branch with same name is absent" {
    local sha

    init_repo
    sha=$(make_commit)
    write_ref "refs/tags/only-tag" "$sha"
    run_rev_parse only-tag
    assert_success
    assert_output_equals "$sha"
}

# annotated tags

@test "qgit rev-parse: annotated tag name resolves to tag object oid" {
    assert_matches_git_annotated_tag_rev_parse v1.0
}

@test "qgit rev-parse: nested annotated tag name resolves to tag object oid" {
    assert_matches_git_annotated_tag_rev_parse release/v1.0 "Nested annotated tag"
}

@test "qgit rev-parse: refs/tags path resolves annotated tag to tag object oid" {
    setup_annotated_tag_ref v1.0 >/dev/null
    assert_matches_git_rev_parse refs/tags/v1.0
}

@test "qgit rev-parse: annotated tag oid differs from tagged commit oid" {
    setup_annotated_tag_ref v1.0 >/dev/null
    assert_matches_git_rev_parse v1.0
    assert_output_not_equals "$REV_PARSE_ANNOTATED_COMMIT"
}

@test "qgit rev-parse: annotated tag matches git for bare name and refs path" {
    setup_annotated_tag_ref release/v2.0 >/dev/null
    assert_matches_git_rev_parse release/v2.0 refs/tags/release/v2.0
}

# peel suffix

@test "qgit rev-parse: annotated tag^{commit} resolves to tagged commit" {
    setup_annotated_peel_fixture v1.0 >/dev/null
    assert_matches_git_rev_parse "v1.0^{commit}"
    assert_output_equals "$REV_PARSE_PEEL_COMMIT"
}

@test "qgit rev-parse: annotated tag^{tree} resolves to commit tree" {
    setup_annotated_peel_fixture v1.0 >/dev/null
    assert_matches_git_rev_parse "v1.0^{tree}"
    assert_output_equals "$REV_PARSE_PEEL_TREE"
}

@test "qgit rev-parse: annotated tag^{tag} resolves to tag object" {
    setup_annotated_peel_fixture v1.0 >/dev/null
    assert_matches_git_rev_parse "v1.0^{tag}"
    assert_output_equals "$REV_PARSE_PEEL_TAG"
}

@test "qgit rev-parse: annotated tag^{} dereferences to tagged commit" {
    setup_annotated_peel_fixture v1.0 >/dev/null
    assert_matches_git_rev_parse "v1.0^{}"
    assert_output_equals "$REV_PARSE_PEEL_COMMIT"
}

@test "qgit rev-parse: refs/tags path with^{commit} resolves to tagged commit" {
    setup_annotated_peel_fixture release/v1.0 "Nested peel tag" >/dev/null
    assert_matches_git_rev_parse "refs/tags/release/v1.0^{commit}"
    assert_output_equals "$REV_PARSE_PEEL_COMMIT"
}

@test "qgit rev-parse: lightweight tag^{} resolves to commit" {
    local sha

    sha=$(setup_branch_head)
    setup_lightweight_tag_ref release "$sha"
    assert_matches_git_rev_parse "release^{}"
    assert_output_equals "$sha"
}

@test "qgit rev-parse: lightweight tag^{commit} resolves to commit" {
    local sha

    sha=$(setup_branch_head)
    setup_lightweight_tag_ref release "$sha"
    assert_matches_git_rev_parse "release^{commit}"
    assert_output_equals "$sha"
}

@test "qgit rev-parse: HEAD^{commit} resolves to branch tip" {
    local sha

    sha=$(setup_branch_head)
    assert_matches_git_rev_parse "HEAD^{commit}"
    assert_output_equals "$sha"
}

@test "qgit rev-parse: HEAD^{tree} resolves to branch tip tree" {
    local sha tree_sha

    sha=$(setup_branch_head)
    tree_sha=$(git_rev_parse "$sha^{tree}")
    assert_matches_git_rev_parse "HEAD^{tree}"
    assert_output_equals "$tree_sha"
}

@test "qgit rev-parse: branch name^{tree} resolves to commit tree" {
    local sha tree_sha

    sha=$(setup_branch_head)
    tree_sha=$(git_rev_parse "$sha^{tree}")
    assert_matches_git_rev_parse "$REV_PARSE_BRANCH^{tree}"
    assert_output_equals "$tree_sha"
}

@test "qgit rev-parse: full sha1^{commit} resolves to itself" {
    local sha

    sha=$(setup_branch_head)
    assert_matches_git_rev_parse "$sha^{commit}"
    assert_output_equals "$sha"
}

@test "qgit rev-parse: full sha1^{tree} resolves to commit tree" {
    local sha tree_sha

    sha=$(setup_branch_head)
    tree_sha=$(git_rev_parse "$sha^{tree}")
    assert_matches_git_rev_parse "$sha^{tree}"
    assert_output_equals "$tree_sha"
}

@test "qgit rev-parse: nested annotated tag^{} dereferences to commit" {
    setup_nested_annotated_tags
    assert_matches_git_rev_parse "outer^{}"
    assert_output_equals "$REV_PARSE_NESTED_COMMIT"
}

@test "qgit rev-parse: nested annotated tag^{commit} resolves to commit" {
    setup_nested_annotated_tags
    assert_matches_git_rev_parse "outer^{commit}"
    assert_output_equals "$REV_PARSE_NESTED_COMMIT"
}

@test "qgit rev-parse: nested annotated tag^{tag} resolves to outer tag object" {
    setup_nested_annotated_tags
    assert_matches_git_rev_parse "outer^{tag}"
    assert_output_equals "$REV_PARSE_NESTED_OUTER"
}

@test "qgit rev-parse: nested annotated inner^{commit} resolves to commit" {
    setup_nested_annotated_tags
    assert_matches_git_rev_parse "inner^{commit}"
    assert_output_equals "$REV_PARSE_NESTED_COMMIT"
}

@test "qgit rev-parse: peel suffix with invalid type fails" {
    setup_annotated_peel_fixture v1.0 >/dev/null
    run_rev_parse "v1.0^{foo}"
    assert_failure
}

@test "qgit rev-parse: peel suffix with trailing text fails" {
    setup_annotated_peel_fixture v1.0 >/dev/null
    run_rev_parse "v1.0^{commit}extra"
    assert_failure
}

@test "qgit rev-parse: annotated tag^{blob} fails when tag points at commit" {
    setup_annotated_peel_fixture v1.0 >/dev/null
    run_rev_parse "v1.0^{blob}"
    assert_failure
    run env GIT_DIR="$(qgit_meta_dir)" "$GIT" rev-parse "v1.0^{blob}"
    assert_failure
}

@test "qgit rev-parse: commit^{blob} fails" {
    local sha

    sha=$(setup_branch_head)
    run_rev_parse "$sha^{blob}"
    assert_failure
}

# branch and tag same name

@test "qgit rev-parse: branch and tag same name same sha succeeds" {
    local sha

    sha=$(setup_branch_head)
    write_ref "refs/tags/$REV_PARSE_BRANCH" "$sha"
    assert_matches_git_rev_parse "$REV_PARSE_BRANCH"
}

@test "qgit rev-parse: branch and tag same name different sha fails" {
    local sha1 sha2

    init_repo
    sha1=$(make_commit "branch commit")
    sha2=$(make_commit "tag commit")
    write_ref "refs/heads/shared-name" "$sha1"
    write_ref "refs/tags/shared-name" "$sha2"
    run_rev_parse shared-name
    assert_failure
}

# short sha1

@test "qgit rev-parse: short sha1 resolves unique object" {
    local sha abbrev

    sha=$(setup_branch_head)
    abbrev=$(abbrev_sha "$sha" 7)
    assert_matches_git_rev_parse "$abbrev"
}

@test "qgit rev-parse: short sha1 minimum length succeeds" {
    local sha abbrev

    sha=$(setup_branch_head)
    abbrev=$(abbrev_sha "$sha" 7)
    assert_matches_git_rev_parse "$abbrev"
}

@test "qgit rev-parse: short sha1 one below minimum length fails" {
    local sha abbrev

    sha=$(setup_branch_head)
    abbrev=$(abbrev_sha "$sha" 6)
    run_rev_parse "$abbrev"
    assert_failure
}

@test "qgit rev-parse: short sha1 shorter than six characters fails" {
    local sha abbrev

    sha=$(setup_branch_head)
    abbrev=$(abbrev_sha "$sha" 3)
    run_rev_parse "$abbrev"
    assert_failure
}

@test "qgit rev-parse: short sha1 maximum length succeeds" {
    local sha abbrev

    sha=$(setup_branch_head)
    abbrev=$(abbrev_sha "$sha" 39)
    assert_matches_git_rev_parse "$abbrev"
}

@test "qgit rev-parse: short sha1 accepts uppercase hex" {
    local sha abbrev upper

    sha=$(setup_branch_head)
    abbrev=$(abbrev_sha "$sha" 7)
    upper=$(uppercase_hex "$abbrev")
    run_rev_parse "$upper"
    assert_success
    assert_output_equals "$sha"
}

@test "qgit rev-parse: short sha1 with no match fails" {
    setup_branch_head
    run_rev_parse deadbeef
    assert_failure
}

@test "qgit rev-parse: short sha1 with non-hex characters fails" {
    setup_branch_head
    run_rev_parse deadcode
    assert_failure
}

@test "qgit rev-parse: short sha1 with missing object directory fails" {
    local sha prefix

    sha=$(setup_branch_head)
    prefix="fffffff"
    [ "${sha:0:7}" = "fffffff" ] && prefix="eeeeeee"
    run_rev_parse "$prefix"
    assert_failure
}

@test "qgit rev-parse: ambiguous short sha1 at minimum length fails" {
    local sha prefix git_dir

    sha=$(setup_branch_head)
    git_dir=$(qgit_meta_dir)
    prefix=$(make_ambiguous_short_prefix "$git_dir" "$sha" 7)
    run_rev_parse "$prefix"
    assert_failure
    run env GIT_DIR="$git_dir" "$GIT" rev-parse "$prefix"
    assert_failure
}

@test "qgit rev-parse: ambiguous short sha1 at longer prefix fails" {
    local sha prefix git_dir

    sha=$(setup_branch_head)
    git_dir=$(qgit_meta_dir)
    prefix=$(make_ambiguous_short_prefix "$git_dir" "$sha" 10)
    run_rev_parse "$prefix"
    assert_failure
    run env GIT_DIR="$git_dir" "$GIT" rev-parse "$prefix"
    assert_failure
}

@test "qgit rev-parse: 39 character hex resolves as short sha1 when unique" {
    local sha abbrev

    sha=$(setup_branch_head)
    abbrev=$(abbrev_sha "$sha" 39)
    assert_matches_git_rev_parse "$abbrev"
}

# multiple args

@test "qgit rev-parse: multiple arguments print one sha per line" {
    local sha abbrev expected

    sha=$(setup_branch_head)
    abbrev=$(abbrev_sha "$sha" 7)
    expected=$(printf '%s\n%s\n%s\n' "$sha" "$sha" "$sha")
    run_rev_parse HEAD "$REV_PARSE_BRANCH" "$abbrev"
    assert_success
    assert_output_equals "$expected"
}

@test "qgit rev-parse: multiple arguments cover all resolution forms" {
    local sha abbrev expected

    sha=$(setup_branch_head)
    write_ref "refs/tags/release" "$sha"
    abbrev=$(abbrev_sha "$sha" 8)
    expected=$(printf '%s\n%s\n%s\n%s\n%s\n%s\n' \
        "$sha" "$sha" "$sha" "$sha" "$sha" "$sha")
    run_rev_parse HEAD "$REV_PARSE_BRANCH" "refs/heads/$REV_PARSE_BRANCH" \
        refs/tags/release "$sha" "$abbrev"
    assert_success
    assert_output_equals "$expected"
}

@test "qgit rev-parse: multiple arguments fail on first bad revision" {
    local sha

    sha=$(setup_branch_head)
    run_rev_parse HEAD deadbeef "$REV_PARSE_BRANCH"
    assert_failure
    assert_output_contains "$sha"
}

@test "qgit rev-parse: multiple arguments fail on first bad revision in front" {
    local sha

    sha=$(setup_branch_head)
    run_rev_parse deadbeef HEAD
    assert_failure
    assert_output_not_contains "$sha"
}

# git parity

@test "qgit rev-parse: matches git for mixed revision forms" {
    local sha abbrev

    sha=$(setup_branch_head)
    write_ref "refs/tags/release" "$sha"
    abbrev=$(abbrev_sha "$sha" 8)
    assert_matches_git_rev_parse HEAD "$REV_PARSE_BRANCH" \
        "refs/heads/$REV_PARSE_BRANCH" refs/tags/release "$sha" "$abbrev"
}

@test "qgit rev-parse: matches git for detached HEAD" {
    local sha

    sha=$(setup_branch_head)
    set_detached_head "$sha"
    assert_matches_git_rev_parse HEAD
}

@test "qgit rev-parse: matches git for nested branch and tag names" {
    local sha

    init_repo
    sha=$(make_commit)
    write_ref "refs/heads/feature/foo" "$sha"
    write_ref "refs/tags/release/v1.0" "$sha"
    assert_matches_git_rev_parse feature/foo release/v1.0
}

# -h

@test "qgit rev-parse -h: shows help" {
    run_rev_parse -h
    assert_success
    assert_output_contains "qgit rev-parse"
    assert_output_contains "Pick out and massage parameters"
}

@test "qgit rev-parse --help: shows help" {
    run_rev_parse --help
    assert_success
    assert_output_contains "qgit rev-parse"
}

# errors

@test "qgit rev-parse: outside repository fails" {
    cd "$TEST_DIR"
    run_rev_parse HEAD
    assert_failure
}

@test "qgit rev-parse: unknown branch name fails" {
    setup_branch_head
    run_rev_parse no-such-branch
    assert_failure
}

@test "qgit rev-parse: non-hex name with no ref fails" {
    setup_branch_head
    run_rev_parse not-a-ref
    assert_failure
}

@test "qgit rev-parse: unknown tag name fails" {
    setup_branch_head
    run_rev_parse no-such-tag
    assert_failure
}

@test "qgit rev-parse: empty argument fails" {
    setup_branch_head
    run_rev_parse ""
    assert_failure
}

@test "qgit rev-parse: unknown flag fails" {
    setup_branch_head
    run_rev_parse --verify HEAD
    assert_failure
}
