# Help output constants and helpers for help.bats and main.bats

HELP_USAGE_LINE="Usage: qgit <command> [options]"
HELP_SUBCMDS_HEADER="Subcommands:"

HELP_SUBCMDS=(
    "add"
    "cat-file"
    "check-ignore"
    "checkout"
    "commit"
    "config"
    "hash-object"
    "help"
    "init"
    "log"
    "ls-files"
    "ls-tree"
    "rev-parse"
    "rm"
    "show-ref"
    "status"
    "tag"
    "version"
)

assert_qgit_help_output() {
    assert_output_contains "$HELP_USAGE_LINE"
    assert_output_contains "$HELP_SUBCMDS_HEADER"
    local cmd
    for cmd in "${HELP_SUBCMDS[@]}"; do
        assert_output_contains "$cmd"
    done
}

run_help() {
    run "$QGIT_BIN" help "$@"
}

run_qgit() {
    run "$QGIT_BIN" "$@"
}
