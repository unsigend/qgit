setup() {
    TEST_DIR=$(mktemp -d)
    cd "$TEST_DIR" || return 1
}

teardown() {
    [ -n "$TEST_DIR" ] && rm -rf "$TEST_DIR"
}