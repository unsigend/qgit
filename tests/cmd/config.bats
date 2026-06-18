load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

setup() {
    TEST_DIR=$(mktemp -d)
    cd "$TEST_DIR" || return 1
    HOME="$TEST_DIR/home"
    mkdir -p "$HOME"
}

teardown() {
    [ -n "$TEST_DIR" ] && rm -rf "$TEST_DIR"
}

# helpers

run_config() {
    run "$QGIT_BIN" config "$@"
}

init_quiet() {
    "$QGIT_BIN" init -q "$@"
}

global_config() {
    printf '%s/.qgitconfig\n' "$HOME"
}

local_config() {
    printf '%s/config\n' "$(qgit_meta_dir)"
}

write_global_config() {
    mkdir -p "$HOME"
    cat > "$(global_config)"
}

write_local_config() {
    cat > "$(local_config)"
}

assert_exit_status() {
    if [ "$status" -ne "$1" ]; then
        echo "Expected exit status $1 but got $status"
        return 1
    fi
}

assert_local_defaults_in_list() {
    assert_output_contains "core.repositoryformatversion=0"
    assert_output_contains "core.filemode=false"
    assert_output_contains "core.bare=false"
}

assert_global_before_local() {
    local global_marker="$1"
    local local_marker="$2"
    local before

    before="${output%%${local_marker}*}"
    [[ "$before" == *"$global_marker"* ]] || {
        echo "Expected '$global_marker' before '$local_marker'"
        echo "Actual output: $output"
        return 1
    }
}

# errors

@test "qgit config: no action fails" {
    run_config
    assert_failure
    assert_output_contains "no action specified"
}

@test "qgit config: multiple actions --get --set fails" {
    run_config --get --set core.a x
    assert_failure
    assert_output_contains "cannot use multiple actions at once"
}

@test "qgit config: multiple actions --list --get fails" {
    run_config --list --get core.a
    assert_failure
    assert_output_contains "cannot use multiple actions at once"
}

@test "qgit config: multiple actions --set --unset fails" {
    run_config --set --unset core.a
    assert_failure
    assert_output_contains "cannot use multiple actions at once"
}

@test "qgit config: conflicting scope --global --local fails" {
    run_config --global --local --list
    assert_failure
    assert_output_contains "cannot use --global and --local together"
}

@test "qgit config --get: invalid key without dot fails" {
    run_config --get foo
    assert_failure
    assert_output_contains "invalid key format"
}

@test "qgit config --get: invalid key with empty section fails" {
    run_config --get .name
    assert_failure
    assert_output_contains "invalid key format"
}

@test "qgit config --get: invalid key with empty name fails" {
    run_config --get core.
    assert_failure
    assert_output_contains "invalid key format"
}

# list

@test "qgit config --global --list: prints global entries" {
    write_global_config <<'EOF'
[core]
	foo=bar
[user]
	name=test
EOF

    run_config --global --list
    assert_success
    assert_output_contains "core.foo=bar"
    assert_output_contains "user.name=test"
}

@test "qgit config --global --list: empty when file missing" {
    run_config --global --list
    assert_success
    assert_output_empty
}

@test "qgit config --global --list: HOME unset fails" {
    run env -u HOME "$QGIT_BIN" config --global --list
    assert_failure
}

@test "qgit config --local --list: prints default local entries" {
    init_quiet

    run_config --local --list
    assert_success
    assert_local_defaults_in_list
}

@test "qgit config --local --list: outside repository fails" {
    run_config --local --list
    assert_failure
    assert_output_contains "inside a qgit repository"
}

@test "qgit config --local --list: empty when config file missing" {
    init_quiet
    rm -f "$(local_config)"

    run_config --local --list
    assert_success
    assert_output_empty
}

@test "qgit config --list: global entries before local entries" {
    write_global_config <<'EOF'
[user]
	name=global
EOF
    init_quiet
    "$QGIT_BIN" config --local --set b.b 2

    run_config --list
    assert_success
    assert_global_before_local "user.name=global" "b.b=2"
    assert_output_contains "core.filemode=false"
}

@test "qgit config --list: global only outside repository" {
    write_global_config <<'EOF'
[core]
	foo=global
EOF

    run_config --list
    assert_success
    assert_output_equals "core.foo=global"
}

@test "qgit config --list: local only without global file" {
    init_quiet

    run_config --list
    assert_success
    assert_local_defaults_in_list
}

@test "qgit config --list: empty without repository or global file" {
    run_config --list
    assert_success
    assert_output_empty
}

# get

@test "qgit config --global --get: returns existing value" {
    write_global_config <<'EOF'
[core]
	foo=bar
EOF

    run_config --global --get core.foo
    assert_success
    assert_output_equals "bar"
}

@test "qgit config --global --get: missing key exits 1 when file present" {
    write_global_config <<'EOF'
[core]
	foo=bar
EOF

    run_config --global --get core.missing
    assert_exit_status 1
}

@test "qgit config --global --get: missing key exits 1 when file missing" {
    run_config --global --get core.foo
    assert_exit_status 1
}

@test "qgit config --global --get: missing key argument fails" {
    run_config --global --get
    assert_failure
    assert_output_contains "--get requires a key"
}

@test "qgit config --local --get: returns existing value" {
    init_quiet

    run_config --local --get core.bare
    assert_success
    assert_output_equals "false"
}

@test "qgit config --local --get: missing key exits 1" {
    init_quiet

    run_config --local --get core.missing
    assert_exit_status 1
}

@test "qgit config --local --get: missing config file exits 1" {
    init_quiet
    rm -f "$(local_config)"

    run_config --local --get core.bare
    assert_exit_status 1
}

@test "qgit config --local --get: outside repository fails" {
    run_config --local --get core.bare
    assert_failure
    assert_output_contains "inside a qgit repository"
}

@test "qgit config --get: prefers local over global" {
    write_global_config <<'EOF'
[core]
	foo=global
EOF
    init_quiet
    "$QGIT_BIN" config --local --set core.foo local

    run_config --get core.foo
    assert_success
    assert_output_equals "local"
}

@test "qgit config --get: falls back to global" {
    write_global_config <<'EOF'
[core]
	foo=global
EOF
    init_quiet

    run_config --get core.foo
    assert_success
    assert_output_equals "global"
}

@test "qgit config --get: missing key exits 1 in repository" {
    init_quiet

    run_config --get core.missing
    assert_exit_status 1
}

@test "qgit config --get: global value outside repository" {
    write_global_config <<'EOF'
[core]
	foo=global
EOF

    run_config --get core.foo
    assert_success
    assert_output_equals "global"
}

@test "qgit config --get: missing key exits 1 outside repository" {
    run_config --get core.foo
    assert_exit_status 1
}

# set

@test "qgit config --global --set: updates existing key" {
    write_global_config <<'EOF'
[core]
	foo=old
	bar=keep
EOF

    run_config --global --set core.foo new
    assert_success
    assert_file_content_contains "$(global_config)" "foo=new"
    assert_file_content_contains "$(global_config)" "bar=keep"
}

@test "qgit config --global --set: creates global file" {
    run_config --global --set core.foo bar
    assert_success
    assert_file_exists "$(global_config)"
    assert_file_content_contains "$(global_config)" "foo=bar"
}

@test "qgit config --global --set: appends new key" {
    write_global_config <<'EOF'
[core]
	foo=bar
EOF

    run_config --global --set user.name test
    assert_success
    assert_file_content_contains "$(global_config)" "foo=bar"
    assert_file_content_contains "$(global_config)" "name=test"
}

@test "qgit config --global --set: missing arguments fails" {
    run_config --global --set core.foo
    assert_failure
    assert_output_contains "--set requires a key and a value"
}

@test "qgit config --local --set: updates local config in repository" {
    init_quiet

    run_config --local --set core.foo x
    assert_success
    assert_file_content_contains "$(local_config)" "foo=x"
    assert_file_content_contains "$(local_config)" "bare=false"
}

@test "qgit config --set: writes local config without scope" {
    init_quiet

    run_config --set core.foo x
    assert_success
    assert_file_content_contains "$(local_config)" "foo=x"
}

@test "qgit config --set: outside repository fails" {
    run_config --set core.foo x
    assert_failure
    assert_output_contains "inside a qgit repository"
}

@test "qgit config --local --set: outside repository fails" {
    run_config --local --set core.foo x
    assert_failure
    assert_output_contains "inside a qgit repository"
}

@test "qgit config --local --set: missing config file fails" {
    init_quiet
    rm -f "$(local_config)"

    run_config --local --set core.foo x
    assert_failure
    assert_output_contains "missing config file in qgit repository"
}

# unset

@test "qgit config --global --unset: removes key and preserves others" {
    write_global_config <<'EOF'
[core]
	foo=bar
	baz=keep
EOF

    run_config --global --unset core.foo
    assert_success
    assert_file_content_not_contains "$(global_config)" "foo=bar"
    assert_file_content_contains "$(global_config)" "baz=keep"
}

@test "qgit config --global --unset: missing key exits 1 when file present" {
    write_global_config <<'EOF'
[core]
	foo=bar
EOF

    run_config --global --unset core.missing
    assert_exit_status 1
}

@test "qgit config --global --unset: exits 0 when file missing" {
    run_config --global --unset core.foo
    assert_exit_status 0
}

@test "qgit config --unset: removes key from local config" {
    init_quiet
    "$QGIT_BIN" config --local --set core.foo bar

    run_config --unset core.foo
    assert_success
    assert_file_content_not_contains "$(local_config)" "foo=bar"
    assert_file_content_contains "$(local_config)" "bare=false"
}

@test "qgit config --local --unset: missing key exits 1" {
    init_quiet

    run_config --local --unset core.missing
    assert_exit_status 1
}

@test "qgit config --unset: outside repository fails" {
    run_config --unset core.foo
    assert_failure
    assert_output_contains "inside a qgit repository"
}

@test "qgit config --local --unset: outside repository fails" {
    run_config --local --unset core.foo
    assert_failure
    assert_output_contains "inside a qgit repository"
}

@test "qgit config --local --unset: missing config file fails" {
    init_quiet
    rm -f "$(local_config)"

    run_config --local --unset core.bare
    assert_failure
}

# integration

@test "qgit config --get: local overrides global after both are set" {
    write_global_config <<'EOF'
[core]
	foo=global
EOF
    init_quiet
    "$QGIT_BIN" config --local --set core.foo local

    run_config --get core.foo
    assert_success
    assert_output_equals "local"
}

@test "qgit config --set: auto scope does not modify global file" {
    write_global_config <<'EOF'
[user]
	name=global
EOF
    init_quiet

    run_config --set core.x 1
    assert_success
    assert_file_content_contains "$(global_config)" "name=global"
    assert_file_content_contains "$(local_config)" "x=1"
}

@test "qgit config --list: ordering with distinct global and local keys" {
    write_global_config <<'EOF'
[a]
	a=1
EOF
    init_quiet
    "$QGIT_BIN" config --local --set b.b 2

    run_config --list
    assert_success
    assert_global_before_local "a.a=1" "b.b=2"
}

@test "qgit config --set then --get: round trip" {
    init_quiet

    run_config --set custom.key val
    assert_success

    run_config --get custom.key
    assert_success
    assert_output_equals "val"
}

@test "qgit config --set then --unset then --get: missing after unset" {
    init_quiet
    "$QGIT_BIN" config --set custom.key val

    run_config --unset custom.key
    assert_success

    run_config --get custom.key
    assert_exit_status 1
}

@test "qgit config --set: key with dot in name round trips" {
    init_quiet

    run_config --set remote.origin.url https://example.com
    assert_success

    run_config --get remote.origin.url
    assert_success
    assert_output_equals "https://example.com"
}

@test "qgit config --get: discovers repository from subdirectory" {
    init_quiet
    mkdir sub
    cd sub || return 1

    run_config --get core.bare
    assert_success
    assert_output_equals "false"
}

# help

@test "qgit config -h: shows help" {
    run_config -h
    assert_success
    assert_output_contains "qgit config"
    assert_output_contains "USAGE:"
}

@test "qgit config --help: shows help" {
    run_config --help
    assert_success
    assert_output_contains "qgit config"
    assert_output_contains "USAGE:"
}

@test "qgit config -h: does not require repository" {
    run_config -h
    assert_success
    assert_qgit_absent
}
