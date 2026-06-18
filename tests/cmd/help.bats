load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"
load "helpers/help.bash"

# helpers

run_help() {
    run "$QGIT_BIN" help "$@"
}

run_qgit() {
    run "$QGIT_BIN" "$@"
}

# help

@test "qgit help: prints usage and subcommands" {
    run_help
    assert_success
    assert_qgit_help_output
}

@test "qgit help: no repository required" {
    run_help
    assert_success
    assert_qgit_help_output
}

@test "qgit help: same output on repeat" {
    run_help
    assert_success
    first="$output"

    run_help
    assert_success
    assert_output_equals "$first"
}

# bare invocation

@test "qgit: no subcommand prints usage and subcommands" {
    run_qgit
    assert_success
    assert_qgit_help_output
}

@test "qgit: no subcommand matches help output" {
    run_qgit
    assert_success
    first="$output"

    run_help
    assert_success
    assert_output_equals "$first"
}

# extra args

@test "qgit help: extra arguments ignored" {
    run_help ignored extra args
    assert_success
    assert_qgit_help_output
}

@test "qgit help: extra arguments do not change output" {
    run_help
    assert_success
    first="$output"

    run_help ignored extra args
    assert_success
    assert_output_equals "$first"
}
