load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "../helpers/commit_messages.bash"
load "helpers/log.bash"

# log

@test "qgit log: linear history matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log
}

@test "qgit log: single commit matches git" {
    setup_linear_history 1 >/dev/null
    assert_matches_git_log
}

@test "qgit log: long linear chain matches git" {
    setup_linear_history 8 >/dev/null
    assert_matches_git_log
}

@test "qgit log: simple merge matches git" {
    setup_merge_first_parent_history >/dev/null
    assert_matches_git_log
}

@test "qgit log: merge includes second parent branch" {
    setup_merge_first_parent_history >/dev/null
    run_qgit_log
    assert_success
    assert_output_contains "second parent branch"
}

@test "qgit log: merge shows Merge line" {
    setup_merge_first_parent_history >/dev/null
    run_qgit_log
    assert_success
    assert_output_contains "Merge:"
}

@test "qgit log: diamond merge matches git" {
    setup_diamond_merge_history >/dev/null
    assert_matches_git_log
}

@test "qgit log: diamond merge includes both branch tips" {
    setup_diamond_merge_history >/dev/null
    run_qgit_log
    assert_success
    assert_output_contains "diamond left tip"
    assert_output_contains "diamond right tip"
}

@test "qgit log: octopus merge matches git" {
    setup_octopus_merge_history >/dev/null
    assert_matches_git_log
}

@test "qgit log: octopus merge includes all branch commits" {
    setup_octopus_merge_history >/dev/null
    run_qgit_log
    assert_success
    assert_output_contains "octopus branch a"
    assert_output_contains "octopus branch b"
    assert_output_contains "octopus branch c"
}

@test "qgit log: nested merge matches git" {
    setup_nested_merge_history >/dev/null
    assert_matches_git_log
}

@test "qgit log: nested merge includes continued side branch" {
    setup_nested_merge_history >/dev/null
    run_qgit_log
    assert_success
    assert_output_contains "nested continued b branch"
}

@test "qgit log: merge history includes more commits than --first-parent" {
    setup_diamond_merge_history >/dev/null
    local all_count fp_count

    all_count=$(git_log --oneline | wc -l | tr -d ' ')
    fp_count=$(git_log --oneline --first-parent | wc -l | tr -d ' ')
    [ "$all_count" -gt "$fp_count" ] || {
        echo "Expected all-parent count ($all_count) > first-parent count ($fp_count)"
        return 1
    }
}

# --oneline

@test "qgit log --oneline: linear history matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log --oneline
}

@test "qgit log --oneline: single commit matches git" {
    setup_linear_history 1 >/dev/null
    assert_matches_git_log --oneline
}

@test "qgit log --oneline: simple merge matches git" {
    setup_merge_first_parent_history >/dev/null
    assert_matches_git_log --oneline
}

@test "qgit log --oneline: diamond merge matches git" {
    setup_diamond_merge_history >/dev/null
    assert_matches_git_log --oneline
}

@test "qgit log --oneline: octopus merge matches git" {
    setup_octopus_merge_history >/dev/null
    assert_matches_git_log --oneline
}

@test "qgit log --oneline: nested merge matches git" {
    setup_nested_merge_history >/dev/null
    assert_matches_git_log --oneline
}

# -n

@test "qgit log -n 1: limits to one commit" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log -n 1
}

@test "qgit log -n 2: limits to two commits" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log -n 2
}

@test "qgit log -n 3: limits to three commits" {
    setup_linear_history 5 >/dev/null
    assert_matches_git_log -n 3
}

@test "qgit log -n 0: shows no commits" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_log -n 0
}

@test "qgit log -n 10: shows all when limit exceeds history" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_log -n 10
}

@test "qgit log -n 1 --oneline: one line limit matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log -n 1 --oneline
}

@test "qgit log -n 2 --oneline: one line limit matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log -n 2 --oneline
}

@test "qgit log -n 4: merge history limit matches git" {
    setup_merge_first_parent_history >/dev/null
    assert_matches_git_log -n 4
}

@test "qgit log -n 1: merge tip limits to one" {
    setup_merge_first_parent_history >/dev/null
    assert_matches_git_log -n 1
}

@test "qgit log -n 6: diamond merge limit matches git" {
    setup_diamond_merge_history >/dev/null
    assert_matches_git_log -n 6
}

# <commit>

@test "qgit log HEAD: explicit HEAD matches git" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_log HEAD
}

@test "qgit log <commit>: starts from middle commit" {
    setup_linear_history 5 >/dev/null
    local mid

    mid=$(commit_at 2)
    assert_matches_git_log "$mid"
}

@test "qgit log <commit>: root commit shows one entry" {
    setup_linear_history 4 >/dev/null
    local root

    root=$(commit_at 0)
    assert_matches_git_log "$root"
}

@test "qgit log --oneline <commit>: middle commit matches git" {
    setup_linear_history 5 >/dev/null
    local mid

    mid=$(commit_at 2)
    assert_matches_git_log --oneline "$mid"
}

@test "qgit log -n 1 <commit>: limits from middle commit" {
    setup_linear_history 5 >/dev/null
    local mid

    mid=$(commit_at 3)
    assert_matches_git_log -n 1 "$mid"
}

@test "qgit log <commit>: second parent branch tip matches git" {
    setup_merge_first_parent_history >/dev/null
    local second_parent

    second_parent=$(
        env GIT_DIR="$TEST_DIR/.git" "$GIT" log --format=%H --grep='second parent branch' -1
    )
    assert_matches_git_log "$second_parent"
}

@test "qgit log <commit>: diamond right branch tip matches git" {
    setup_diamond_merge_history >/dev/null
    local right_tip

    right_tip=$(
        env GIT_DIR="$TEST_DIR/.git" "$GIT" log --format=%H --grep='diamond right tip' -1
    )
    assert_matches_git_log "$right_tip"
}

# name resolution

@test "qgit log: branch name resolves start commit" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_log "$LOG_BRANCH"
}

@test "qgit log: refs/heads path resolves start commit" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_log "refs/heads/$LOG_BRANCH"
}

@test "qgit log: short sha1 resolves start commit" {
    local sha abbrev

    sha=$(setup_linear_history 3)
    abbrev=$(abbrev_sha "$sha" 7)
    assert_matches_git_log "$abbrev"
}

@test "qgit log --oneline: branch name matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log --oneline "$LOG_BRANCH"
}

@test "qgit log -n 2: branch name limit matches git" {
    setup_linear_history 5 >/dev/null
    assert_matches_git_log -n 2 "$LOG_BRANCH"
}

# message edge cases

@test "qgit log: empty commit message matches git" {
    setup_empty_message_log >/dev/null
    assert_matches_git_log
}

@test "qgit log: multiline message matches git" {
    setup_multiline_log_history >/dev/null
    assert_matches_git_log
}

@test "qgit log --oneline: multiline message uses first line only" {
    setup_multiline_log_history >/dev/null
    run_qgit_log --oneline
    assert_success
    assert_output_contains "line one"
    assert_output_not_contains "line two"
    assert_output_not_contains "line three"
}

@test "qgit log: message starting with author matches git" {
    setup_message_log "author note only" >/dev/null
    assert_matches_git_log
}

@test "qgit log: message starting with committer matches git" {
    setup_message_log "committer note only" >/dev/null
    assert_matches_git_log
}

@test "qgit log: message starting with tree matches git" {
    setup_message_log "tree rebuild notes" >/dev/null
    assert_matches_git_log
}

@test "qgit log: message starting with parent matches git" {
    setup_message_log "parent linkage notes" >/dev/null
    assert_matches_git_log
}

# time and timezone

@test "qgit log: timezone offsets match git" {
    setup_timezone_history >/dev/null
    assert_matches_git_log
}

@test "qgit log --oneline: timezone offsets match git" {
    setup_timezone_history >/dev/null
    assert_matches_git_log --oneline
}

@test "qgit log: different author and committer times match git" {
    setup_different_author_committer_history >/dev/null
    assert_matches_git_log
}

@test "qgit log --oneline: different author and committer times match git" {
    setup_different_author_committer_history >/dev/null
    assert_matches_git_log --oneline
}

# committer date ordering

@test "qgit log: merge sibling order uses committer date like git" {
    setup_committer_order_merge_history >/dev/null
    assert_matches_git_log
}

@test "qgit log --oneline: merge sibling order uses committer date like git" {
    setup_committer_order_merge_history >/dev/null
    assert_matches_git_log --oneline
}

@test "qgit log: octopus sibling order uses committer date like git" {
    setup_committer_order_octopus_history >/dev/null
    assert_matches_git_log
}

@test "qgit log --oneline: octopus sibling order uses committer date like git" {
    setup_committer_order_octopus_history >/dev/null
    assert_matches_git_log --oneline
}

# --first-parent

@test "qgit log --first-parent: linear history matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: single commit matches git" {
    setup_linear_history 1 >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: merge follows first parent only" {
    setup_merge_first_parent_history >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: merge skips second parent branch" {
    setup_merge_first_parent_history >/dev/null
    run_qgit_log --first-parent
    assert_success
    assert_output_not_contains "second parent branch"
}

@test "qgit log --first-parent: diamond merge matches git" {
    setup_diamond_merge_history >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: octopus merge matches git" {
    setup_octopus_merge_history >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: nested merge matches git" {
    setup_nested_merge_history >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: long linear chain matches git" {
    setup_linear_history 8 >/dev/null
    assert_matches_git_log --first-parent
}

# --oneline --first-parent

@test "qgit log --oneline --first-parent: linear history matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log --oneline --first-parent
}

@test "qgit log --oneline --first-parent: single commit matches git" {
    setup_linear_history 1 >/dev/null
    assert_matches_git_log --oneline --first-parent
}

@test "qgit log --oneline --first-parent: merge matches git" {
    setup_merge_first_parent_history >/dev/null
    assert_matches_git_log --oneline --first-parent
}

# -n --first-parent

@test "qgit log -n 1 --first-parent: limits to one commit" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log -n 1 --first-parent
}

@test "qgit log -n 2 --first-parent: limits to two commits" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log -n 2 --first-parent
}

@test "qgit log -n 3 --first-parent: limits to three commits" {
    setup_linear_history 5 >/dev/null
    assert_matches_git_log -n 3 --first-parent
}

@test "qgit log -n 0 --first-parent: shows no commits" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_log -n 0 --first-parent
}

@test "qgit log -n 10 --first-parent: shows all when limit exceeds history" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_log -n 10 --first-parent
}

@test "qgit log -n 1 --oneline --first-parent: one line limit matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log -n 1 --oneline --first-parent
}

@test "qgit log -n 2 --oneline --first-parent: one line limit matches git" {
    setup_linear_history 4 >/dev/null
    assert_matches_git_log -n 2 --oneline --first-parent
}

@test "qgit log -n 1 --first-parent: merge tip limits to one" {
    setup_merge_first_parent_history >/dev/null
    assert_matches_git_log -n 1 --first-parent
}

@test "qgit log -n 2 --oneline --first-parent: merge limit matches git" {
    setup_merge_first_parent_history >/dev/null
    assert_matches_git_log -n 2 --oneline --first-parent
}

# <commit> --first-parent

@test "qgit log --first-parent HEAD: explicit HEAD matches git" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_log --first-parent HEAD
}

@test "qgit log --first-parent <commit>: starts from middle commit" {
    setup_linear_history 5 >/dev/null
    local mid

    mid=$(commit_at 2)
    assert_matches_git_log --first-parent "$mid"
}

@test "qgit log --first-parent <commit>: root commit shows one entry" {
    setup_linear_history 4 >/dev/null
    local root

    root=$(commit_at 0)
    assert_matches_git_log --first-parent "$root"
}

@test "qgit log --oneline --first-parent <commit>: middle commit matches git" {
    setup_linear_history 5 >/dev/null
    local mid

    mid=$(commit_at 2)
    assert_matches_git_log --oneline --first-parent "$mid"
}

@test "qgit log -n 1 --first-parent <commit>: limits from middle commit" {
    setup_linear_history 5 >/dev/null
    local mid

    mid=$(commit_at 3)
    assert_matches_git_log -n 1 --first-parent "$mid"
}

@test "qgit log --first-parent: branch name resolves start commit" {
    setup_linear_history 3 >/dev/null
    assert_matches_git_log --first-parent "$LOG_BRANCH"
}

# --first-parent message edge cases

@test "qgit log --first-parent: empty commit message matches git" {
    setup_empty_message_log >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: multiline message matches git" {
    setup_multiline_log_history >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --oneline --first-parent: multiline message uses first line only" {
    setup_multiline_log_history >/dev/null
    run_qgit_log --oneline --first-parent
    assert_success
    assert_output_contains "line one"
    assert_output_not_contains "line two"
    assert_output_not_contains "line three"
}

# --first-parent time and timezone

@test "qgit log --first-parent: timezone offsets match git" {
    setup_timezone_history >/dev/null
    assert_matches_git_log --first-parent
}

@test "qgit log --first-parent: different author and committer times match git" {
    setup_different_author_committer_history >/dev/null
    assert_matches_git_log --first-parent
}

# errors

@test "qgit log: outside repository fails" {
    mkdir outside && cd outside || return 1
    run_qgit_log
    assert_failure
}

@test "qgit log: unknown reference fails" {
    setup_linear_history 2 >/dev/null
    run_qgit_log not-a-ref
    assert_failure
}

@test "qgit log: missing object fails" {
    setup_linear_history 2 >/dev/null
    run_qgit_log 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit log: non-commit object fails" {
    setup_linear_history 2 >/dev/null
    local blob_sha

    blob_sha=$(git_write_blob blob-v0.txt)
    run_qgit_log "$blob_sha"
    assert_failure
}

@test "qgit log -n: missing count fails" {
    setup_linear_history 2 >/dev/null
    run_qgit_log -n
    assert_failure
}

@test "qgit log --first-parent: outside repository fails" {
    mkdir outside && cd outside || return 1
    run_qgit_log --first-parent
    assert_failure
}

@test "qgit log --first-parent: unknown reference fails" {
    setup_linear_history 2 >/dev/null
    run_qgit_log --first-parent not-a-ref
    assert_failure
}

@test "qgit log --first-parent: non-commit object fails" {
    setup_linear_history 2 >/dev/null
    local blob_sha

    blob_sha=$(git_write_blob blob-v0.txt)
    run_qgit_log --first-parent "$blob_sha"
    assert_failure
}

# peel suffix

@test "qgit log: annotated tag name matches git" {
    setup_annotated_tag_on_tip v1.0 >/dev/null
    assert_matches_git_log v1.0
}

@test "qgit log: annotated tag^{commit} matches git" {
    setup_annotated_tag_on_tip v1.0 >/dev/null
    assert_matches_git_log "v1.0^{commit}"
}

@test "qgit log: annotated tag^{} matches git" {
    setup_annotated_tag_on_tip v1.0 >/dev/null
    assert_matches_git_log "v1.0^{}"
}

@test "qgit log: nested annotated tag name matches git" {
    setup_annotated_tag_on_tip release/v1.0 "Nested release" >/dev/null
    assert_matches_git_log release/v1.0
}

@test "qgit log: HEAD^{commit} matches git" {
    setup_linear_history 2 >/dev/null
    assert_matches_git_log "HEAD^{commit}"
}

@test "qgit log: annotated tag^{tag} matches git" {
    setup_annotated_tag_on_tip v1.0 >/dev/null
    assert_matches_git_log "v1.0^{tag}"
}

# -h

@test "qgit log -h: shows help" {
    run_qgit_log -h
    assert_success
    assert_output_contains "qgit log"
    assert_output_contains "--first-parent"
}

@test "qgit log --help: shows help" {
    run_qgit_log --help
    assert_success
    assert_output_contains "qgit log"
    assert_output_contains "--oneline"
}
