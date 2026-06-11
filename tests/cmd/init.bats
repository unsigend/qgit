load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

# helpers

run_init() {
    run "$QGIT_BIN" init "$@"
}

init_quiet() {
    "$QGIT_BIN" init -q "$@"
}

mark_description() {
    echo "custom marker" >"$(qgit_meta_dir "$1")/description"
}

# init

@test "qgit init: creates repo layout in cwd" {
    run_init
    assert_success
    assert_qgit_repo_layout
}

@test "qgit init: HEAD points to default branch" {
    run_init
    assert_success
    assert_qgit_head "$QGIT_DEFAULT_BRANCH"
}

@test "qgit init: config defaults" {
    run_init
    assert_success
    assert_qgit_config_defaults
}

@test "qgit init: description file" {
    run_init
    assert_success
    assert_qgit_description
}

@test "qgit init: prints initialized message" {
    run_init
    assert_success
    assert_output_matches "^Initialized empty qgit repository in .+"
}

# path

@test "qgit init <path>: creates repo at path" {
    mkdir target
    run_init target
    assert_success
    assert_qgit_repo_layout target
}

@test "qgit init <path>: leaves cwd untouched" {
    mkdir target
    run_init target
    assert_success
    assert_qgit_absent "."
}

@test "qgit init: creates nested directories" {
    run_init nested/deep/repo
    assert_success
    assert_qgit_repo_layout nested/deep/repo
}

@test "qgit init: resolves relative path" {
    mkdir -p foo/bar
    run_init ./foo/../bar/repo
    assert_success
    assert_qgit_repo_layout bar/repo
}

@test "qgit init: accepts absolute path" {
    local target="$TEST_DIR/abs-repo"
    run_init "$target"
    assert_success
    assert_qgit_repo_layout "$target"
}

@test "qgit init: existing empty directory" {
    mkdir empty
    run_init empty
    assert_success
    assert_qgit_repo_layout empty
}

@test "qgit init: existing directory with files" {
    mkdir mixed
    echo content >mixed/readme
    run_init mixed
    assert_success
    assert_qgit_repo_layout mixed
    assert_file_content_equals mixed/readme "content"
}

@test "qgit init: path is file fails" {
    touch myfile
    run_init myfile
    assert_failure
}

# -b

@test "qgit init -b: sets initial branch" {
    run_init -b dev
    assert_success
    assert_qgit_head dev
}

@test "qgit init --initial-branch: sets initial branch" {
    run_init --initial-branch feature
    assert_success
    assert_qgit_head feature
}

@test "qgit init -b: with path argument" {
    mkdir branch-target
    run_init -b dev branch-target
    assert_success
    assert_qgit_head dev branch-target
}

@test "qgit init -b: path before flags" {
    mkdir order-target
    run_init order-target -b dev
    assert_success
    assert_qgit_head dev order-target
}

@test "qgit init -b: empty branch name fails" {
    run_init -b ""
    assert_failure
    assert_output_contains "invalid branch name"
}

@test "qgit init -b: missing value fails" {
    run_init -b
    assert_failure
}

# -q

@test "qgit init -q: no output" {
    run_init -q
    assert_success
    assert_output_empty
}

@test "qgit init -q: still creates repo" {
    run_init -q
    assert_success
    assert_qgit_repo_layout
    assert_qgit_head "$QGIT_DEFAULT_BRANCH"
}

@test "qgit init -q -b: no output with custom branch" {
    run_init -q -b dev
    assert_success
    assert_output_empty
    assert_qgit_head dev
}

# reinit

@test "qgit init: reinit prints message" {
    init_quiet
    run_init
    assert_success
    assert_output_contains "Reinitialized existing qgit repository in"
}

@test "qgit init: reinit preserves existing files" {
    init_quiet
    mark_description
    run_init
    assert_success
    assert_file_content_contains "$(qgit_meta_dir)/description" "custom marker"
}

@test "qgit init: reinit preserves config edits" {
    init_quiet
    "$QGIT_BIN" config --set core.bare true
    run_init
    assert_success
    assert_file_content_contains "$(qgit_meta_dir)/config" "bare=true"
}

@test "qgit init -b: does not overwrite HEAD on reinit" {
    init_quiet
    run_init -b dev
    assert_success
    assert_qgit_head "$QGIT_DEFAULT_BRANCH"
}

@test "qgit init -q: reinit produces no output" {
    init_quiet
    run_init -q
    assert_success
    assert_output_empty
}

# -h

@test "qgit init -h: shows help" {
    run_init -h
    assert_success
    assert_output_contains "qgit init"
    assert_output_contains "initial-branch"
}

@test "qgit init --help: shows help" {
    run_init --help
    assert_success
    assert_output_contains "qgit init"
    assert_output_contains "quiet"
}

# errors

@test "qgit init: unknown flag fails" {
    run_init --unknown-flag
    assert_failure
}

@test "qgit init: parent path is not directory fails" {
    touch blocked
    run_init blocked/sub
    assert_failure
}

@test "qgit init: nested repo inside existing repo" {
    init_quiet
    run_init inner
    assert_success
    assert_qgit_repo_layout
    assert_qgit_repo_layout inner
}

@test "qgit init: second init in same path succeeds" {
    run_init
    assert_success
    run_init
    assert_success
}
