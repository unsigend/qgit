load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

# helpers

VERSION_PATTERN='^qgit version [0-9]+\.[0-9]+\.[0-9]+$'

run_version() {
    run "$QGIT_BIN" version "$@"
}

assert_version_output() {
    assert_output_matches "$VERSION_PATTERN"
}

# version

@test "qgit version: prints version string" {
    run_version
    assert_success
    assert_version_output
}

@test "qgit version: output has no embedded newline" {
    run_version
    assert_success
    [[ "$output" != *$'\n'* ]] || {
        echo "Expected single line output"
        echo "Actual output: $output"
        return 1
    }
}

@test "qgit version: ignores extra arguments" {
    run_version --bogus extra
    assert_success
    assert_version_output
}

@test "qgit version: same output on repeat" {
    run_version
    assert_success
    expected="$output"

    run_version
    assert_success
    assert_output_equals "$expected"
}

# extra args

@test "qgit version: unknown flag is ignored" {
    run_version --unknown-flag
    assert_success
    assert_version_output
}

@test "qgit version: -h flag is ignored" {
    run_version -h
    assert_success
    assert_version_output
}
