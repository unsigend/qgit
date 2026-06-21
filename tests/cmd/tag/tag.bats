load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "helpers/tag.bash"

# tag

@test "qgit tag: outside repository fails" {
    run_tag v1.0.0
    assert_failure
}

@test "qgit tag: create lightweight tag at HEAD" {
    local sha

    sha=$(setup_branch_head "head commit")
    run_tag v1.0.0
    assert_success
    assert_output_empty
    assert_tag_ref_equals "v1.0.0" "$sha"
}

@test "qgit tag: create tag at explicit commit" {
    local sha other

    sha=$(setup_branch_head "target commit")
    other=$(make_commit "other commit")
    run_tag release "$other"
    assert_success
    assert_output_empty
    assert_tag_ref_equals "release" "$other"
}

@test "qgit tag: create tag at explicit commit hash" {
    local sha

    sha=$(setup_branch_head "hash target")
    run_tag by-sha "$sha"
    assert_success
    assert_tag_ref_equals "by-sha" "$sha"
}

@test "qgit tag: create tag using branch name as commit" {
    local sha

    sha=$(setup_branch_head "branch target")
    run_tag from-branch "$TAG_BRANCH"
    assert_success
    assert_tag_ref_equals "from-branch" "$sha"
}

@test "qgit tag: create nested tag name" {
    local sha

    sha=$(setup_branch_head "nested commit")
    run_tag release/v1.0.0
    assert_success
    assert_tag_ref_equals "release/v1.0.0" "$sha"
}

@test "qgit tag: matches git create at HEAD" {
    local sha

    sha=$(setup_branch_head "git create head")
    assert_matches_git_tag_create v1.0.0 "$sha"
}

@test "qgit tag: matches git create at explicit commit" {
    local sha other

    sha=$(setup_branch_head "git create explicit")
    other=$(make_commit "git other commit")
    assert_matches_git_tag_create release "$other"
}

@test "qgit tag: matches git create and list" {
    local sha

    sha=$(setup_branch_head "git parity")
    git_tag v1.0.0 "$sha"
    run_tag v2.0.0 "$sha"
    assert_success
    assert_matches_git_tag_list
}

@test "qgit tag: duplicate tag without force fails" {
    local sha second

    sha=$(setup_branch_head "first commit")
    run_tag stable "$sha"
    assert_success
    second=$(make_commit "second commit")
    run_tag stable "$second"
    assert_failure
    assert_tag_ref_equals "stable" "$sha"
}

@test "qgit tag: fails when HEAD does not resolve" {
    init_repo
    run_tag v1.0.0
    assert_failure
}

@test "qgit tag: fails with unresolvable commit" {
    setup_branch_head "known commit"
    run_tag v1.0.0 no-such-branch
    assert_failure
}

@test "qgit tag: create without tag name fails" {
    setup_branch_head "missing tag name"
    run_tag -f
    assert_failure
}

@test "qgit tag: multiple tags at different commits" {
    local first second third

    first=$(setup_branch_head "first release")
    second=$(make_commit "second release")
    third=$(make_commit "third release")
    run_tag v1.0.0 "$first"
    assert_success
    run_tag v1.1.0 "$second"
    assert_success
    run_tag v1.2.0 "$third"
    assert_success
    assert_tag_ref_equals "v1.0.0" "$first"
    assert_tag_ref_equals "v1.1.0" "$second"
    assert_tag_ref_equals "v1.2.0" "$third"
    assert_matches_git_tag_list_ordered
}

# -d

@test "qgit tag -d: delete existing tag" {
    local sha

    sha=$(setup_branch_head "delete me")
    run_tag v1.0.0 "$sha"
    assert_success
    run_tag -d v1.0.0
    assert_success
    assert_output_empty
    [ ! -f "$(tag_ref_file v1.0.0)" ]
}

@test "qgit tag --delete: delete existing tag" {
    local sha

    sha=$(setup_branch_head "delete me")
    run_tag v1.0.0 "$sha"
    assert_success
    run_tag --delete v1.0.0
    assert_success
    [ ! -f "$(tag_ref_file v1.0.0)" ]
}

@test "qgit tag -d: delete nested tag" {
    local sha

    sha=$(setup_branch_head "nested delete")
    run_tag release/v1.0.0 "$sha"
    assert_success
    run_tag -d release/v1.0.0
    assert_success
    [ ! -f "$(tag_ref_file release/v1.0.0)" ]
}

@test "qgit tag -d: matches git delete" {
    local sha

    sha=$(setup_branch_head "git delete")
    git_tag v1.0.0 "$sha"
    git_tag beta "$sha"
    git_tag -d v1.0.0
    run_tag -d beta
    assert_success
    assert_matches_git_tag_list
}

@test "qgit tag -d: missing tag name fails" {
    setup_branch_head "delete setup"
    run_tag -d
    assert_failure
}

@test "qgit tag -d: missing tag fails" {
    setup_branch_head "delete setup"
    run_tag -d missing-tag
    assert_failure
}

# -f

@test "qgit tag -f: force overwrite existing tag" {
    local sha second

    sha=$(setup_branch_head "first commit")
    run_tag stable "$sha"
    assert_success
    second=$(make_commit "second commit")
    run_tag -f stable "$second"
    assert_success
    assert_tag_ref_equals "stable" "$second"
}

@test "qgit tag --force: force overwrite existing tag" {
    local sha second

    sha=$(setup_branch_head "first commit")
    run_tag stable "$sha"
    assert_success
    second=$(make_commit "second commit")
    run_tag --force stable "$second"
    assert_success
    assert_tag_ref_equals "stable" "$second"
}

@test "qgit tag -f: matches git force overwrite" {
    local sha second

    sha=$(setup_branch_head "git force first")
    git_tag stable "$sha"
    second=$(make_commit "git force second")
    git_tag -f stable "$second"
    run_tag -f stable "$second"
    assert_success
    assert_tag_ref_equals "stable" "$second"
}

# list

@test "qgit tag: list with no tags produces no output" {
    init_repo
    run_tag
    assert_success
    assert_output_empty
}

@test "qgit tag: list all tags" {
    local sha

    sha=$(setup_branch_head "list commit")
    run_tag alpha "$sha"
    assert_success
    run_tag beta "$sha"
    assert_success
    run_tag
    assert_success
    assert_output_contains "alpha"
    assert_output_contains "beta"
}

@test "qgit tag -l: list all tags" {
    local sha

    sha=$(setup_branch_head "list commit")
    run_tag alpha "$sha"
    assert_success
    run_tag beta "$sha"
    assert_success
    run_tag -l
    assert_success
    assert_output_contains "alpha"
    assert_output_contains "beta"
}

@test "qgit tag --list: list all tags" {
    local sha

    sha=$(setup_branch_head "list commit")
    run_tag alpha "$sha"
    assert_success
    run_tag --list
    assert_success
    assert_output_contains "alpha"
}

@test "qgit tag: list prints tag names only" {
    local sha

    sha=$(setup_branch_head "list commit")
    run_tag release/v1.0.0 "$sha"
    assert_success
    run_tag
    assert_success
    assert_output_equals "release/v1.0.0"
    assert_output_not_contains "refs/tags/"
}

@test "qgit tag: list ten tags matches git output" {
    local sha i name

    sha=$(setup_branch_head "ten tags")
    for i in $(seq 1 10); do
        printf -v name 'tag-%02d' "$i"
        git_tag "$name" "$sha"
    done
    assert_matches_git_tag_list
}

@test "qgit tag: list ten tags from qgit create matches git output" {
    local sha i name

    sha=$(setup_branch_head "ten qgit tags")
    for i in $(seq 1 10); do
        printf -v name 'tag-%02d' "$i"
        run_tag "$name" "$sha"
        assert_success
    done
    assert_matches_git_tag_list
}

@test "qgit tag: list matches git output" {
    local sha

    sha=$(setup_branch_head "git list")
    git_tag alpha "$sha"
    git_tag beta "$sha"
    git_tag release/v1.0.0 "$sha"
    assert_matches_git_tag_list
}

@test "qgit tag -l: matches git list with multiple tags" {
    local sha

    sha=$(setup_branch_head "git list flag")
    git_tag alpha "$sha"
    git_tag beta "$sha"
    assert_matches_git_tag_list -l
}

@test "qgit tag: list works when tags directory is missing" {
    local sha

    sha=$(setup_branch_head "missing tags dir")
    rmdir "$(qgit_meta_dir)/refs/tags"
    run_tag
    assert_success
    assert_output_empty
}

@test "qgit tag -l: ignores extra positional arguments" {
    local sha

    sha=$(setup_branch_head "list extra args")
    run_tag alpha "$sha"
    assert_success
    run_tag -l alpha
    assert_success
    assert_output_equals "alpha"
}

# list order

@test "qgit tag: list version tags in lexicographic order matches git" {
    local sha

    sha=$(setup_branch_head "version tag order")
    run_tag v1.3.0 "$sha"
    assert_success
    run_tag v1.0.0 "$sha"
    assert_success
    run_tag v1.1.0 "$sha"
    assert_success
    run_tag v2.0.0 "$sha"
    assert_success
    assert_matches_git_tag_list_ordered
}

@test "qgit tag: list version tags from git create matches git order" {
    local sha

    sha=$(setup_branch_head "git version tag order")
    git_tag v1.3.0 "$sha"
    git_tag v1.0.0 "$sha"
    git_tag v1.1.0 "$sha"
    git_tag v2.0.0 "$sha"
    assert_matches_git_tag_list_ordered
}

@test "qgit tag: list preserves git order after out-of-order creates" {
    local sha

    sha=$(setup_branch_head "out of order creates")
    run_tag v10.0.0 "$sha"
    assert_success
    run_tag v1.0.0 "$sha"
    assert_success
    run_tag v2.0.0 "$sha"
    assert_success
    run_tag v1.10.0 "$sha"
    assert_success
    assert_matches_git_tag_list_ordered
}

@test "qgit tag: list remaining tags after delete matches git order" {
    local sha

    sha=$(setup_branch_head "list after delete")
    run_tag v1.0.0 "$sha"
    assert_success
    run_tag v1.1.0 "$sha"
    assert_success
    run_tag v1.2.0 "$sha"
    assert_success
    run_tag v2.0.0 "$sha"
    assert_success
    run_tag -d v1.1.0
    assert_success
    assert_matches_git_tag_list_ordered
}

@test "qgit tag -l: list version tags in lexicographic order matches git" {
    local sha

    sha=$(setup_branch_head "version tag order flag")
    run_tag v1.3.0 "$sha"
    assert_success
    run_tag v1.0.0 "$sha"
    assert_success
    run_tag v1.1.0 "$sha"
    assert_success
    assert_matches_git_tag_list_ordered -l
}

# -h

@test "qgit tag -h: shows help" {
    run_tag -h
    assert_success
    assert_output_contains "qgit tag"
    assert_output_contains "-d"
    assert_output_contains "-l"
    assert_output_contains "-f"
    assert_output_contains "<tagname>"
}

@test "qgit tag --help: shows help" {
    run_tag --help
    assert_success
    assert_output_contains "qgit tag"
    assert_output_contains "Create, list, or delete tags"
}

# errors

@test "qgit tag: unknown flag fails" {
    init_repo
    run_tag --unknown-flag
    assert_failure
}

@test "qgit tag: create without repository fails" {
    run_tag v1.0.0 HEAD
    assert_failure
}
