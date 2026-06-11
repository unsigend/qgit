load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

# helpers

setup() {
    TEST_DIR=$(mktemp -d)
    export HOME="$TEST_DIR"
    cd "$TEST_DIR" || return 1
}

init_repo() {
    "$QGIT_BIN" init -q
}

run_config() {
    run "$QGIT_BIN" config "$@"
}

global_config() {
    printf '%s\n' "$HOME/.qgitconfig"
}

local_config() {
    printf '%s\n' ".qgit/config"
}

# --list

@test "qgit config --list: auto in repo prints local entries" {
    init_repo
    run_config --list
    assert_success
    assert_output_contains "core.repositoryformatversion=0"
    assert_output_contains "core.filemode=false"
    assert_output_contains "core.bare=false"
}

@test "qgit config --list --global: prints global entries only" {
    "$QGIT_BIN" config --set --global user.name global
    init_repo
    run_config --list --global
    assert_success
    assert_output_contains "user.name=global"
    assert_output_not_contains "core.bare="
}

@test "qgit config --list --local: prints local entries only" {
    init_repo
    "$QGIT_BIN" config --set --global user.name global
    run_config --list --local
    assert_success
    assert_output_contains "core.bare=false"
    assert_output_not_contains "user.name=global"
}

@test "qgit config --list: auto outside repo prints global only" {
    "$QGIT_BIN" config --set --global user.email test@example.com
    mkdir outside && cd outside || return 1
    run_config --list
    assert_success
    assert_output_contains "user.email=test@example.com"
    assert_output_not_contains "core.bare="
}

@test "qgit config --list: auto prints global before local" {
    "$QGIT_BIN" config --set --global user.name global-only
    init_repo
    run_config --list
    assert_success
    assert_output_contains "user.name=global-only"
    assert_output_contains "core.bare=false"
    local before_local="${output%%core.bare=false*}"
    [[ "$before_local" =~ user.name=global-only ]]
}

@test "qgit config --list: silent when no config exists" {
    run_config --list
    assert_success
    assert_output_empty
}

@test "qgit config -l: short form lists entries" {
    init_repo
    run_config -l
    assert_success
    assert_output_contains "core.bare=false"
}

# --get

@test "qgit config --get: auto reads local value in repo" {
    init_repo
    run_config --get core.bare
    assert_success
    assert_output_equals "false"
}

@test "qgit config --get --local: reads local config only" {
    init_repo
    "$QGIT_BIN" config --set --global core.bare true
    run_config --get --local core.bare
    assert_success
    assert_output_equals "false"
}

@test "qgit config --get --global: reads global config only" {
    init_repo
    "$QGIT_BIN" config --set --global user.name global
    "$QGIT_BIN" config --set user.name local
    run_config --get --global user.name
    assert_success
    assert_output_equals "global"
}

@test "qgit config --get: auto prefers local over global" {
    init_repo
    "$QGIT_BIN" config --set --global user.name global
    "$QGIT_BIN" config --set user.name local
    run_config --get user.name
    assert_success
    assert_output_equals "local"
}

@test "qgit config --get: auto falls back to global" {
    init_repo
    "$QGIT_BIN" config --set --global user.name global-only
    run_config --get user.name
    assert_success
    assert_output_equals "global-only"
}

@test "qgit config --get: auto outside repo reads global" {
    "$QGIT_BIN" config --set --global user.email outside@example.com
    mkdir outside && cd outside || return 1
    run_config --get user.email
    assert_success
    assert_output_equals "outside@example.com"
}

@test "qgit config --get: missing key exits 1" {
    init_repo
    run_config --get core.missing
    assert_failure
    assert_output_empty
}

@test "qgit config -g: short form gets value" {
    init_repo
    run_config -g core.bare
    assert_success
    assert_output_equals "false"
}

# --set

@test "qgit config --set: auto writes local config in repo" {
    init_repo
    run_config --set core.bare true
    assert_success
    assert_file_content_contains "$(local_config)" "bare=true"
    run_config --get core.bare
    assert_success
    assert_output_equals "true"
}

@test "qgit config --set --local: writes local config" {
    init_repo
    "$QGIT_BIN" config --set --global core.bare true
    run_config --set --local core.bare local-value
    assert_success
    assert_file_content_contains "$(local_config)" "bare=local-value"
    run_config --get --global core.bare
    assert_success
    assert_output_equals "true"
}

@test "qgit config --set --global: writes global config" {
    run_config --set --global user.name foo
    assert_success
    assert_file_exists "$(global_config)"
    assert_file_content_contains "$(global_config)" "name=foo"
    run_config --get --global user.name
    assert_success
    assert_output_equals "foo"
}

@test "qgit config --set --global: works outside repo" {
    mkdir outside && cd outside || return 1
    run_config --set --global user.name remote
    assert_success
    run_config --get --global user.name
    assert_success
    assert_output_equals "remote"
}

@test "qgit config -s: short form sets value" {
    init_repo
    run_config -s user.email test@example.com
    assert_success
    run_config --get user.email
    assert_success
    assert_output_equals "test@example.com"
}

# --unset

@test "qgit config --unset: auto removes key from local config" {
    init_repo
    "$QGIT_BIN" config --set core.bare true
    run_config --unset core.bare
    assert_success
    assert_file_content_not_contains "$(local_config)" "bare=true"
    run_config --get core.bare
    assert_failure
}

@test "qgit config --unset --local: removes key from local config" {
    init_repo
    "$QGIT_BIN" config --set --local core.bare true
    run_config --unset --local core.bare
    assert_success
    assert_file_content_not_contains "$(local_config)" "bare=true"
    run_config --get --local core.bare
    assert_failure
}

@test "qgit config --unset --global: removes key from global config" {
    "$QGIT_BIN" config --set --global user.name foo
    run_config --unset --global user.name
    assert_success
    run_config --get --global user.name
    assert_failure
}

@test "qgit config --unset: auto outside repo does not change global" {
    "$QGIT_BIN" config --set --global user.name foo
    mkdir outside && cd outside || return 1
    run_config --unset user.name
    assert_success
    assert_output_empty
    run_config --get --global user.name
    assert_success
    assert_output_equals "foo"
}

@test "qgit config --unset: auto does not remove global key" {
    init_repo
    "$QGIT_BIN" config --set --global user.name global-only
    run_config --unset user.name
    assert_failure
    run_config --get --global user.name
    assert_success
    assert_output_equals "global-only"
}

@test "qgit config --unset: missing local key exits 1" {
    init_repo
    cp "$(local_config)" "$(local_config).bak"
    run_config --unset core.missing
    assert_failure
    assert_file_content_equals "$(local_config)" "$(cat "$(local_config).bak")"
}

@test "qgit config -u: short form unsets value" {
    init_repo
    "$QGIT_BIN" config --set user.email test@example.com
    run_config -u user.email
    assert_success
    run_config --get user.email
    assert_failure
}

# scope

@test "qgit config --local --global: cannot combine scope flags" {
    init_repo
    run_config --local --global --list
    assert_failure
    assert_output_contains "--local and --global cannot be used together"
}

@test "qgit config --local: requires repository" {
    run_config --local --list
    assert_failure
    assert_output_contains "--local can only be used inside a qgit repository"
}

@test "qgit config --local --get: fails without valid local config" {
    mkdir repo && mkdir repo/.qgit && cd repo || return 1
    run_config --local --get core.bare
    assert_failure
    assert_output_contains "--local can only be used inside a qgit repository"
}

# errors

@test "qgit config: requires an action" {
    init_repo
    run_config
    assert_failure
    assert_output_contains "no action specified"
}

@test "qgit config --set: requires repository without scope" {
    run_config --set core.bare true
    assert_failure
    assert_output_contains "qgit repository may broken"
}

@test "qgit config --set: requires key and value" {
    init_repo
    run_config --set core.bare
    assert_failure
    assert_output_contains "--set requires a key and a value"
}

@test "qgit config --get: requires key" {
    init_repo
    run_config --get
    assert_failure
    assert_output_contains "--get requires a key"
}

@test "qgit config --unset: requires key" {
    init_repo
    run_config --unset
    assert_failure
    assert_output_contains "--unset requires a key"
}

@test "qgit config --get: rejects invalid key format" {
    init_repo
    run_config --get bare
    assert_failure
    assert_output_contains "invalid key format"
}

@test "qgit config --set: rejects invalid key format" {
    init_repo
    run_config --set bare true
    assert_failure
    assert_output_contains "invalid key format"
}

@test "qgit config --unset: rejects invalid key format" {
    init_repo
    run_config --unset bare
    assert_failure
    assert_output_contains "invalid key format"
}

# -h

@test "qgit config -h: shows help" {
    run_config -h
    assert_success
    assert_output_contains "qgit config"
    assert_output_contains "--global"
}

@test "qgit config --help: shows help" {
    run_config --help
    assert_success
    assert_output_contains "qgit config"
    assert_output_contains "--unset"
}
