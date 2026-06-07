load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

@test "qgit version" {
    run "$QGIT_BIN" version
    assert_success
    assert_output_not_empty
}

@test "qgit version: proper format" {
    run "$QGIT_BIN" version
    assert_success
    assert_output_matches "^qgit version [0-9]+\.[0-9]+\.[0-9]+$"
}
