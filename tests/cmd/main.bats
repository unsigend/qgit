load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"
load "helpers/help.bash"

# helpers

UNKNOWN_CMD_PATTERN="^qgit: '.+' is not a qgit command\. See 'qgit help' for available commands\.$"

run_qgit() {
    run "$QGIT_BIN" "$@"
}

# default

@test "qgit: no args shows help" {
    run_qgit
    assert_success
    assert_qgit_help_output
}

@test "qgit: no args matches qgit help" {
    run_qgit
    assert_success
    expected="$output"

    run_qgit help
    assert_success
    assert_output_equals "$expected"
}

# errors

@test "qgit: unknown command fails" {
    run_qgit non-exist
    assert_failure
    assert_output_contains "is not a qgit command"
}

@test "qgit: unknown command suggests help" {
    run_qgit bogus-command
    assert_failure
    assert_output_contains "See 'qgit help' for available commands"
}

@test "qgit: unknown command message format" {
    run_qgit missing-cmd
    assert_failure
    assert_output_matches "$UNKNOWN_CMD_PATTERN"
}

@test "qgit: unknown command with args fails" {
    run_qgit bad-cmd --flag value
    assert_failure
}

@test "qgit: empty command token fails" {
    run_qgit ""
    assert_failure
}
