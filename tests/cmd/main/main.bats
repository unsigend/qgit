load "../helpers/setup.bash"
load "../helpers/helpers.bash"
load "../helpers/globals.bash"
load "helpers/main.bash"

# invalid command

@test "qgit invalid-cmd: exits with status 1" {
    run_qgit invalid-cmd
    assert_exit_status 1
}

@test "qgit unknown: exits with status 1" {
    run_qgit unknown
    assert_exit_status 1
}

@test "qgit invalid-cmd: no repository required" {
    run_qgit invalid-cmd
    assert_exit_status 1
}

@test "qgit invalid-cmd: extra arguments still fail" {
    run_qgit invalid-cmd extra args
    assert_exit_status 1
}

@test "qgit invalid-cmd: reports unknown command" {
    run_qgit invalid-cmd
    assert_exit_status 1
    assert_output_contains "not a qgit command"
}

@test "qgit invalid-cmd: suggests help" {
    run_qgit invalid-cmd
    assert_exit_status 1
    assert_output_contains "qgit help"
}

# errors

@test "qgit invalid-cmd: does not print usage" {
    run_qgit invalid-cmd
    assert_exit_status 1
    assert_output_not_contains "Usage: qgit <command> [options]"
}

@test "qgit invalid-cmd: same exit status on repeat" {
    run_qgit invalid-cmd
    assert_exit_status 1
    first="$status"

    run_qgit invalid-cmd
    assert_exit_status 1
    [ "$status" -eq "$first" ] || {
        echo "Expected repeat exit status $first but got $status"
        return 1
    }
}
