load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"
load "helpers/help.bash"

# helpers

run_help() {
    run "$QGIT_BIN" help "$@"
}

# help

@test "qgit help: prints usage and subcommands" {
    run_help
    assert_success
    assert_qgit_help_output
}

@test "qgit help: lists command descriptions" {
    run_help
    assert_success
    assert_output_contains "Add file contents to the index"
    assert_output_contains "Create an empty qgit repository or reinitialize an existing one"
    assert_output_contains "Display help information about qgit"
    assert_output_contains "Show the version"
}

@test "qgit help: ignores extra arguments" {
    run_help init bogus
    assert_success
    assert_qgit_help_output
}

@test "qgit help: same output on repeat" {
    run_help
    assert_success
    expected="$output"

    run_help
    assert_success
    assert_output_equals "$expected"
}

# default

@test "qgit help: same output as bare qgit" {
    run "$QGIT_BIN"
    assert_success
    expected="$output"

    run_help
    assert_success
    assert_output_equals "$expected"
}

# extra args

@test "qgit help: unknown flag is ignored" {
    run_help --unknown-flag
    assert_success
    assert_qgit_help_output
}

@test "qgit help: -h flag is ignored" {
    run_help -h
    assert_success
    assert_qgit_help_output
}
