load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

@test "qgit" {
    run "$QGIT_BIN"
    assert_success
    assert_output_not_empty
    assert_output_contains "Usage: qgit <command> [options]"
    assert_output_contains "Subcommands:"
}

@test "qgit non-exist" {
    run "$QGIT_BIN" non-exist
    assert_failure
    assert_output_contains "'non-exist' is not a qgit command"
}

@test "qgit non-exist: suggests help" {
    run "$QGIT_BIN" bogus-command
    assert_failure
    assert_output_contains "See 'qgit help' for available commands"
}
