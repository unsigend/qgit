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

assert_qgit_initialized() {
    local root="${1:-.}"
    local branch="${2:-$QGIT_DEFAULT_BRANCH}"

    assert_qgit_repo_layout "$root"
    assert_qgit_head "$branch" "$root"
    assert_qgit_config_defaults "$root"
    assert_qgit_description "$root"
}

assert_qgit_config_exact() {
    local root="${1:-.}"
    local config
    config="$(qgit_meta_dir "$root")/config"

    assert_file_content_contains "$config" "[core]"
    assert_file_content_contains "$config" "repositoryformatversion=0"
    assert_file_content_contains "$config" "filemode=false"
    assert_file_content_contains "$config" "bare=false"
}

# init

@test "qgit init: creates repository layout in current directory" {
    assert_qgit_absent

    run_init
    assert_success
    assert_qgit_initialized
}

@test "qgit init: creates expected directories" {
    run_init
    assert_success

    local meta
    meta=$(qgit_meta_dir)
    assert_dir_exists "$meta"
    assert_dir_exists "$meta/objects"
    assert_dir_exists "$meta/refs/heads"
    assert_dir_exists "$meta/refs/tags"
}

@test "qgit init: creates expected files" {
    run_init
    assert_success

    local meta
    meta=$(qgit_meta_dir)
    assert_file_exists "$meta/HEAD"
    assert_file_exists "$meta/config"
    assert_file_exists "$meta/description"
}

@test "qgit init: HEAD points to default branch" {
    run_init
    assert_success
    assert_qgit_head "$QGIT_DEFAULT_BRANCH"
}

@test "qgit init: description has default content" {
    run_init
    assert_success
    assert_qgit_description
}

@test "qgit init: config has default entries" {
    run_init
    assert_success
    assert_qgit_config_exact
}

@test "qgit init: prints initialized message" {
    run_init
    assert_success
    assert_output_contains "Initialized empty repository in "
    assert_output_contains ".qgit"
}

@test "qgit init: does not remove existing files in worktree" {
    echo "keep me" > existing.txt

    run_init
    assert_success
    assert_file_content_contains existing.txt "keep me"
}

@test "qgit init: succeeds in existing empty directory" {
    mkdir emptydir

    run_init emptydir
    assert_success
    assert_qgit_initialized emptydir
}

# path

@test "qgit init <path>: creates repository in target directory" {
    run_init myrepo
    assert_success
    assert_qgit_initialized myrepo
}

@test "qgit init <path>: leaves cwd untouched" {
    run_init nested/repo
    assert_success
    assert_qgit_absent .
    assert_qgit_initialized nested/repo
}

@test "qgit init <path>: creates missing target directory" {
    run_init newdir
    assert_success
    assert_dir_exists newdir
    assert_qgit_initialized newdir
}

# -b

@test "qgit init -b: uses custom branch name" {
    run_init -b dev
    assert_success
    assert_qgit_initialized . dev
}

@test "qgit init --initial-branch: uses custom branch name" {
    run_init --initial-branch=feature
    assert_success
    assert_qgit_initialized . feature
}

@test "qgit init -b: applies to repository in target path" {
    run_init -b release repo-b
    assert_success
    assert_qgit_initialized repo-b release
}

@test "qgit init -b: branch name with slash" {
    run_init -b feature/login
    assert_success
    assert_qgit_head "feature/login"
}

@test "qgit init -q -b: quiet with custom branch" {
    run_init -q -b develop
    assert_success
    assert_output_empty
    assert_qgit_head develop
}

# -q

@test "qgit init -q: no output" {
    run_init -q
    assert_success
    assert_output_empty
    assert_qgit_initialized
}

@test "qgit init --quiet: no output" {
    run_init --quiet
    assert_success
    assert_output_empty
    assert_qgit_initialized
}

@test "qgit init -q: still creates repository layout" {
    run_init -q
    assert_success
    assert_qgit_repo_layout
    assert_qgit_head "$QGIT_DEFAULT_BRANCH"
    assert_qgit_description
    assert_qgit_config_exact
}

# reinit

@test "qgit init reinit: succeeds in existing repository" {
    init_quiet
    run_init
    assert_success
    assert_output_contains "Reinitialized existing repository in "
}

@test "qgit init reinit: does not overwrite HEAD" {
    init_quiet
    echo "ref: refs/heads/keep" > "$(qgit_meta_dir)/HEAD"

    run_init -b other
    assert_success
    assert_file_content_contains "$(qgit_meta_dir)/HEAD" "refs/heads/keep"
}

@test "qgit init reinit: does not overwrite description" {
    init_quiet
    echo "Custom description" > "$(qgit_meta_dir)/description"

    run_init
    assert_success
    assert_file_content_contains "$(qgit_meta_dir)/description" \
        "Custom description"
}

@test "qgit init reinit: does not overwrite config" {
    init_quiet
    echo "custom=value" >> "$(qgit_meta_dir)/config"

    run_init
    assert_success
    assert_file_content_contains "$(qgit_meta_dir)/config" "custom=value"
    assert_qgit_config_defaults
}

@test "qgit init reinit -q: no output" {
    init_quiet
    run_init -q
    assert_success
    assert_output_empty
}

@test "qgit init reinit <path>: succeeds in existing target repository" {
    init_quiet repo
    run_init repo
    assert_success
    assert_output_contains "Reinitialized existing repository in "
    assert_qgit_initialized repo
}

@test "qgit init reinit: recreates missing objects directory" {
    init_quiet
    rm -rf "$(qgit_meta_dir)/objects"

    run_init
    assert_success
    assert_dir_exists "$(qgit_meta_dir)/objects"
    assert_qgit_head "$QGIT_DEFAULT_BRANCH"
}

@test "qgit init reinit: recreates missing description with default" {
    init_quiet
    rm -f "$(qgit_meta_dir)/description"

    run_init
    assert_success
    assert_qgit_description
}

@test "qgit init reinit: recreates missing HEAD with branch option" {
    init_quiet
    rm -f "$(qgit_meta_dir)/HEAD"

    run_init -b restored
    assert_success
    assert_qgit_head restored
}

# -h

@test "qgit init -h: shows help" {
    run_init -h
    assert_success
    assert_output_contains "qgit init"
    assert_output_contains "USAGE:"
}

@test "qgit init --help: shows help" {
    run_init --help
    assert_success
    assert_output_contains "qgit init"
    assert_output_contains "USAGE:"
}

@test "qgit init -h: does not create repository" {
    run_init -h
    assert_success
    assert_qgit_absent
}

@test "qgit init -h: lists quiet and branch options" {
    run_init -h
    assert_success
    assert_output_contains "quiet"
    assert_output_contains "initial-branch"
}

# errors

@test "qgit init -b: empty branch name fails" {
    run_init -b ""
    assert_failure
    assert_qgit_absent
}

@test "qgit init: too many arguments fails" {
    run_init dir1 dir2
    assert_failure
    assert_qgit_absent dir1
}

@test "qgit init: unknown option fails" {
    run_init --unknown
    assert_failure
    assert_qgit_absent
}

@test "qgit init -b: missing branch name fails" {
    run_init -b
    assert_failure
    assert_qgit_absent
}

@test "qgit init <path>: target is existing file fails" {
    touch not-a-dir

    run_init not-a-dir
    assert_failure
    assert_qgit_absent .
}
