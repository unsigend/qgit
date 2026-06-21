setup_blob_from_file() {
    local file="$1"
    local hash

    init_mock_git
    hash=$(git_write_blob "$file")
    sync_git_to_qgit
    echo "$hash"
}
