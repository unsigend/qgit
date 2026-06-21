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
