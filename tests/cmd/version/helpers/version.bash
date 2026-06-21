QGIT_VERSION_PATTERN='^qgit version [0-9]+\.[0-9]+\.[0-9]+$'

run_version() {
    run "$QGIT_BIN" version "$@"
}

assert_qgit_version_output() {
    assert_output_matches "$QGIT_VERSION_PATTERN"
}
