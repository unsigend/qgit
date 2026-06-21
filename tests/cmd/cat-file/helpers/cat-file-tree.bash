setup_single_file_tree() {
    init_mock_git

    local blob_sha tree_sha
    printf 'hello tree\n' >blob-tree.txt
    blob_sha=$(git_write_blob blob-tree.txt)
    tree_sha=$(printf '100644 blob %s\tfile.txt\n' "$blob_sha" | git_write_tree)

    sync_git_to_qgit
    echo "$tree_sha"
}

setup_empty_tree() {
    init_mock_git

    local tree_sha
    tree_sha=$(printf '' | git_write_tree)

    sync_git_to_qgit
    echo "$tree_sha"
}

setup_multi_entry_tree() {
    init_mock_git

    local blob_sha_a blob_sha_b tree_sha
    printf 'alpha\n' >blob-alpha.txt
    printf 'zebra\n' >blob-zebra.txt
    blob_sha_a=$(git_write_blob blob-alpha.txt)
    blob_sha_b=$(git_write_blob blob-zebra.txt)
    tree_sha=$(
        printf '100644 blob %s\talpha.txt\n100644 blob %s\tzebra.txt\n' \
            "$blob_sha_a" "$blob_sha_b" | git_write_tree
    )

    sync_git_to_qgit
    echo "$tree_sha"
}

setup_nested_tree() {
    init_mock_git

    local blob_sha child_sha tree_sha
    printf 'nested\n' >blob-nested.txt
    blob_sha=$(git_write_blob blob-nested.txt)
    child_sha=$(printf '100644 blob %s\tinner.txt\n' "$blob_sha" | git_write_tree)
    tree_sha=$(printf '040000 tree %s\tsubdir\n' "$child_sha" | git_write_tree)

    sync_git_to_qgit
    echo "$tree_sha"
}

setup_mixed_tree() {
    init_mock_git

    local root_blob_sha sub_blob_sha child_sha tree_sha
    printf 'root file\n' >root.txt
    printf 'sub file\n' >sub.txt
    root_blob_sha=$(git_write_blob root.txt)
    sub_blob_sha=$(git_write_blob sub.txt)
    child_sha=$(printf '100644 blob %s\tinner.txt\n' "$sub_blob_sha" | git_write_tree)
    tree_sha=$(
        printf '100644 blob %s\tREADME\n040000 tree %s\tdocs\n' \
            "$root_blob_sha" "$child_sha" | git_write_tree
    )

    sync_git_to_qgit
    echo "$tree_sha"
}

setup_executable_tree() {
    init_mock_git

    local blob_sha tree_sha
    printf '#!/bin/sh\necho hi\n' >script.sh
    blob_sha=$(git_write_blob script.sh)
    tree_sha=$(printf '100755 blob %s\tscript.sh\n' "$blob_sha" | git_write_tree)

    sync_git_to_qgit
    echo "$tree_sha"
}

setup_symlink_tree() {
    init_mock_git

    local link_sha tree_sha
    link_sha=$(printf 'target.txt' | env GIT_DIR="$TEST_DIR/.git" "$GIT" hash-object -w --stdin)
    tree_sha=$(printf '120000 blob %s\tlink.txt\n' "$link_sha" | git_write_tree)

    sync_git_to_qgit
    echo "$tree_sha"
}
