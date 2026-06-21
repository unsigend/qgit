run_qgit() {
    run "$QGIT_BIN" "$@" 2>&1
}

assert_exit_status() {
    if [ "$status" -ne "$1" ]; then
        echo "Expected exit status $1 but got $status"
        return 1
    fi
}
