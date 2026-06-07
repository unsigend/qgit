load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

setup() {
    TEST_DIR=$(mktemp -d)
    export HOME="$TEST_DIR"
    cd "$TEST_DIR" || return 1
}

init_repo() {
    "$QGIT_BIN" init -q
}

@test "qgit config --list: prints local entries in repo" {
    init_repo
    run "$QGIT_BIN" config --list
    assert_success
    assert_output_contains "core.repositoryformatversion=0"
    assert_output_contains "core.filemode=false"
    assert_output_contains "core.bare=false"
}

@test "qgit config --list --global: prints global entries" {
    "$QGIT_BIN" config --set --global user.name global
    run "$QGIT_BIN" config --list --global
    assert_success
    assert_output_contains "user.name=global"
}

@test "qgit config --list --local: prints local entries" {
    init_repo
    run "$QGIT_BIN" config --list --local
    assert_success
    assert_output_contains "core.bare=false"
}

@test "qgit config --list: silent when no config files exist" {
    run "$QGIT_BIN" config --list
    assert_success
    assert_output_empty
}

@test "qgit config --list: auto outside repo prints global only" {
    "$QGIT_BIN" config --set --global user.email test@example.com
    mkdir outside
    cd outside || return 1
    run "$QGIT_BIN" config --list
    assert_success
    assert_output_contains "user.email=test@example.com"
    assert_output_not_contains "core.bare="
}

@test "qgit config --list: auto prints global before local" {
    "$QGIT_BIN" config --set --global user.name global-only
    init_repo
    run "$QGIT_BIN" config --list
    assert_success
    assert_output_contains "user.name=global-only"
    assert_output_contains "core.bare=false"
    local before_local="${output%%core.bare=false*}"
    if [[ ! "$before_local" =~ user.name=global-only ]]; then
        echo "Expected global entries before local entries"
        echo "Actual output: $output"
        return 1
    fi
}

@test "qgit config --get core.bare: prints value in repo" {
    init_repo
    run "$QGIT_BIN" config --get core.bare
    assert_success
    assert_output_equals "false"
}

@test "qgit config --get: key not found exits 1" {
    init_repo
    run "$QGIT_BIN" config --get core.missing
    assert_failure
    assert_output_empty
}

@test "qgit config --get --global: reads global config only" {
    init_repo
    "$QGIT_BIN" config --set --global user.name global
    "$QGIT_BIN" config --set user.name local
    run "$QGIT_BIN" config --get --global user.name
    assert_success
    assert_output_equals "global"
}

@test "qgit config --get --local: reads local config only" {
    init_repo
    "$QGIT_BIN" config --set --global core.bare true
    run "$QGIT_BIN" config --get --local core.bare
    assert_success
    assert_output_equals "false"
}

@test "qgit config --get: local takes precedence over global" {
    init_repo
    "$QGIT_BIN" config --set --global user.name global
    "$QGIT_BIN" config --set user.name local
    run "$QGIT_BIN" config --get user.name
    assert_success
    assert_output_equals "local"
}

@test "qgit config --get: auto falls back to global" {
    init_repo
    "$QGIT_BIN" config --set --global user.name global-only
    run "$QGIT_BIN" config --get user.name
    assert_success
    assert_output_equals "global-only"
}

@test "qgit config --get: dies without key" {
    init_repo
    run "$QGIT_BIN" config --get
    assert_failure
    assert_output_contains "--get requires a key"
}

@test "qgit config --get: dies with invalid key format" {
    init_repo
    run "$QGIT_BIN" config --get bare
    assert_failure
    assert_output_contains "invalid key format"
}

@test "qgit config --set core.bare true: updates local config" {
    init_repo
    run "$QGIT_BIN" config --set core.bare true
    assert_success
    assert_file_content_contains ".qgit/config" "bare=true"
    run "$QGIT_BIN" config --get core.bare
    assert_success
    assert_output_equals "true"
}

@test "qgit config --set --local core.bare true: updates local config" {
    init_repo
    "$QGIT_BIN" config --set --global core.bare true
    run "$QGIT_BIN" config --set --local core.bare local-value
    assert_success
    assert_file_content_contains ".qgit/config" "bare=local-value"
    run "$QGIT_BIN" config --get --global core.bare
    assert_success
    assert_output_equals "true"
}

@test "qgit config --set --global user.name foo: writes global config" {
    run "$QGIT_BIN" config --set --global user.name foo
    assert_success
    assert_file_exists "$HOME/.qgitconfig"
    assert_file_content_contains "$HOME/.qgitconfig" "name=foo"
    run "$QGIT_BIN" config --get --global user.name
    assert_success
    assert_output_equals "foo"
}

@test "qgit config --set: dies outside repo" {
    run "$QGIT_BIN" config --set core.bare true
    assert_failure
    assert_output_contains "qgit repository may broken"
}

@test "qgit config --set: dies without value" {
    init_repo
    run "$QGIT_BIN" config --set core.bare
    assert_failure
    assert_output_contains "--set requires a key and a value"
}

@test "qgit config --set: dies with invalid key format" {
    init_repo
    run "$QGIT_BIN" config --set bare true
    assert_failure
    assert_output_contains "invalid key format"
}

@test "qgit config --unset core.bare: removes key from local config" {
    init_repo
    "$QGIT_BIN" config --set core.bare true
    run "$QGIT_BIN" config --unset core.bare
    assert_success
    assert_file_content_not_contains ".qgit/config" "bare=true"
    run "$QGIT_BIN" config --get core.bare
    assert_failure
}

@test "qgit config --unset --local core.bare: removes key from local config" {
    init_repo
    "$QGIT_BIN" config --set --local core.bare true
    run "$QGIT_BIN" config --unset --local core.bare
    assert_success
    assert_file_content_not_contains ".qgit/config" "bare=true"
    run "$QGIT_BIN" config --get --local core.bare
    assert_failure
}

@test "qgit config --unset: auto outside repo exits 0 silently" {
    "$QGIT_BIN" config --set --global user.name foo
    mkdir outside
    cd outside || return 1
    run "$QGIT_BIN" config --unset user.name
    assert_success
    assert_output_empty
    run "$QGIT_BIN" config --get --global user.name
    assert_success
    assert_output_equals "foo"
}

@test "qgit config --unset: key not found exits 1" {
    init_repo
    cp .qgit/config .qgit/config.bak
    run "$QGIT_BIN" config --unset core.missing
    assert_failure
    assert_file_content_equals ".qgit/config" "$(cat .qgit/config.bak)"
}

@test "qgit config --unset --global user.name: removes from global config" {
    "$QGIT_BIN" config --set --global user.name foo
    run "$QGIT_BIN" config --unset --global user.name
    assert_success
    run "$QGIT_BIN" config --get --global user.name
    assert_failure
}

@test "qgit config --unset: dies without key" {
    init_repo
    run "$QGIT_BIN" config --unset
    assert_failure
    assert_output_contains "--unset requires a key"
}

@test "qgit config --local --global: dies together" {
    init_repo
    run "$QGIT_BIN" config --local --global --list
    assert_failure
    assert_output_contains "--local and --global cannot be used together"
}

@test "qgit config --local: dies outside repo" {
    run "$QGIT_BIN" config --local --list
    assert_failure
    assert_output_contains "--local can only be used inside a qgit repository"
}

@test "qgit config: dies with no action specified" {
    init_repo
    run "$QGIT_BIN" config
    assert_failure
    assert_output_contains "no action specified"
}

@test "qgit config --local --get: dies when local config unavailable" {
    mkdir repo
    mkdir repo/.qgit
    cd repo || return 1
    run "$QGIT_BIN" config --local --get core.bare
    assert_failure
    assert_output_contains "--local can only be used inside a qgit repository"
}
