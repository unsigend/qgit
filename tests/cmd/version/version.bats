load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "helpers/version.bash"

# version

@test "qgit version: prints version" {
    run_version
    assert_success
    assert_qgit_version_output
}

@test "qgit version: no repository required" {
    run_version
    assert_success
    assert_qgit_version_output
}

@test "qgit version: same output on repeat" {
    run_version
    assert_success
    first="$output"

    run_version
    assert_success
    assert_output_equals "$first"
}

# extra args

@test "qgit version: extra arguments ignored" {
    run_version ignored extra args
    assert_success
    assert_qgit_version_output
}

@test "qgit version: extra arguments do not change output" {
    run_version
    assert_success
    first="$output"

    run_version ignored extra args
    assert_success
    assert_output_equals "$first"
}
