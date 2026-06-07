load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

@test "qgit help" {
    run "$QGIT_BIN" help
    assert_success
    assert_output_not_empty
}

@test "qgit help: shows usage" {
    run "$QGIT_BIN" help
    assert_success
    assert_output_contains "Usage: qgit <command> [options]"
}

@test "qgit help: lists subcommands" {
    run "$QGIT_BIN" help
    assert_success
    assert_output_contains "Subcommands:"
    assert_output_contains "add"
    assert_output_contains "init"
    assert_output_contains "version"
}

@test "qgit help: same output as qgit" {
    run "$QGIT_BIN"
    assert_success
    expected="$output"

    run "$QGIT_BIN" help
    assert_success
    assert_output_equals "$expected"
}
