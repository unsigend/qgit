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
