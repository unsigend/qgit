store_malformed_commit_payload() {
    local file="$1"
    local sha type

    init_repo
    sha=$("$QGIT_BIN" hash-object -w -t commit "$file")
    if [ -z "$sha" ]; then
        echo "qgit hash-object failed to store malformed commit" >&2
        return 1
    fi

    type=$("$QGIT_BIN" cat-file -t "$sha")
    if [ "$type" != "commit" ]; then
        echo "expected stored commit object, got type: $type" >&2
        return 1
    fi

    printf '%s\n' "$sha"
}

write_commit_missing_tree() {
    local file="$1"

    cat >"$file" <<EOF
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

missing tree line
EOF
}

write_commit_missing_author() {
    local file="$1"

    cat >"$file" <<EOF
tree $EMPTY_TREE_SHA
committer Test User <test@example.com> 946684800 +0000

missing author line
EOF
}

write_commit_missing_committer() {
    local file="$1"

    cat >"$file" <<EOF
tree $EMPTY_TREE_SHA
author Test User <test@example.com> 946684800 +0000

missing committer line
EOF
}

write_commit_missing_all_headers() {
    local file="$1"

    cat >"$file" <<EOF

message only
EOF
}

write_commit_unknown_header() {
    local file="$1"

    cat >"$file" <<EOF
tree $EMPTY_TREE_SHA
invalid header value
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

unknown header line
EOF
}

write_commit_duplicate_tree() {
    local file="$1"

    cat >"$file" <<EOF
tree $EMPTY_TREE_SHA
tree $EMPTY_TREE_SHA
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

duplicate tree lines
EOF
}

write_commit_duplicate_author() {
    local file="$1"

    cat >"$file" <<EOF
tree $EMPTY_TREE_SHA
author Test User <test@example.com> 946684800 +0000
author Another User <other@example.com> 946684900 +0000
committer Test User <test@example.com> 946684800 +0000

duplicate author lines
EOF
}

write_commit_duplicate_committer() {
    local file="$1"

    cat >"$file" <<EOF
tree $EMPTY_TREE_SHA
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000
committer Another User <other@example.com> 946684900 +0000

duplicate committer lines
EOF
}

write_commit_bad_author_signature() {
    local file="$1"

    cat >"$file" <<EOF
tree $EMPTY_TREE_SHA
author bad signature line
committer Test User <test@example.com> 946684800 +0000

bad author signature
EOF
}

write_commit_bad_committer_signature() {
    local file="$1"

    cat >"$file" <<EOF
tree $EMPTY_TREE_SHA
author Test User <test@example.com> 946684800 +0000
committer bad signature line

bad committer signature
EOF
}

assert_cat_file_p_rejects_malformed_commit() {
    local file="$1"
    local hash

    hash=$(store_malformed_commit_payload "$file")
    run_cat_file -p "$hash"
    assert_failure
}

write_tree_for_blob() {
    local blob_file="$1"
    local entry_name="$2"
    local blob_sha

    blob_sha=$(git_write_blob "$blob_file")
    printf '100644 blob %s\t%s\n' "$blob_sha" "$entry_name" | git_write_tree
}

finalize_mock_commit() {
    local commit_file="$1"
    local commit_sha

    commit_sha=$(git_write_commit "$commit_file")
    sync_git_to_qgit
    echo "$commit_sha"
}

setup_root_commit() {
    init_mock_git

    local blob_sha tree_sha commit_sha
    printf 'hello world\n' >blob-root.txt
    blob_sha=$(git_write_blob blob-root.txt)
    tree_sha=$(printf '100644 blob %s\tfile.txt\n' "$blob_sha" | git_write_tree)
    cat >commit-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

Root commit message
EOF
    commit_sha=$(git_write_commit commit-root.txt)

    sync_git_to_qgit
    echo "$commit_sha"
}

setup_commit_with_branch_ref() {
    local sha

    sha=$(setup_root_commit)
    write_ref "refs/heads/$QGIT_DEFAULT_BRANCH" "$sha"
    set_symref_head "$QGIT_DEFAULT_BRANCH"
    printf '%s\n' "$sha"
}

setup_commit_with_parent() {
    init_mock_git

    local blob_sha tree_sha root_sha child_sha
    printf 'base content\n' >blob-base.txt
    blob_sha=$(git_write_blob blob-base.txt)
    tree_sha=$(printf '100644 blob %s\tbase.txt\n' "$blob_sha" | git_write_tree)
    cat >commit-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

initial commit
EOF
    root_sha=$(git_write_commit commit-root.txt)

    printf 'child content\n' >blob-child.txt
    blob_sha=$(git_write_blob blob-child.txt)
    tree_sha=$(printf '100644 blob %s\tchild.txt\n' "$blob_sha" | git_write_tree)
    cat >commit-child.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0000
committer Test User <test@example.com> 946771200 +0000

second commit
EOF
    child_sha=$(git_write_commit commit-child.txt)

    sync_git_to_qgit
    echo "$child_sha"
}

setup_merge_commit() {
    init_mock_git

    local blob_sha tree_sha root_sha commit_a_sha commit_b_sha merge_sha
    printf 'base\n' >blob-base.txt
    blob_sha=$(git_write_blob blob-base.txt)
    tree_sha=$(printf '100644 blob %s\tbase.txt\n' "$blob_sha" | git_write_tree)
    cat >commit-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

root
EOF
    root_sha=$(git_write_commit commit-root.txt)

    printf 'branch-a\n' >blob-a.txt
    blob_sha=$(git_write_blob blob-a.txt)
    tree_sha=$(printf '100644 blob %s\ta.txt\n' "$blob_sha" | git_write_tree)
    cat >commit-a.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0000
committer Test User <test@example.com> 946771200 +0000

branch a
EOF
    commit_a_sha=$(git_write_commit commit-a.txt)

    printf 'branch-b\n' >blob-b.txt
    blob_sha=$(git_write_blob blob-b.txt)
    tree_sha=$(printf '100644 blob %s\tb.txt\n' "$blob_sha" | git_write_tree)
    cat >commit-b.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946857600 +0000
committer Test User <test@example.com> 946857600 +0000

branch b
EOF
    commit_b_sha=$(git_write_commit commit-b.txt)

    printf 'merged\n' >blob-merge.txt
    blob_sha=$(git_write_blob blob-merge.txt)
    tree_sha=$(printf '100644 blob %s\tmerged.txt\n' "$blob_sha" | git_write_tree)
    cat >commit-merge.txt <<EOF
tree $tree_sha
parent $commit_a_sha
parent $commit_b_sha
author Test User <test@example.com> 946944000 +0000
committer Test User <test@example.com> 946944000 +0000

merge branches a and b
EOF
    merge_sha=$(git_write_commit commit-merge.txt)

    sync_git_to_qgit
    echo "$merge_sha"
}

setup_multiline_commit() {
    init_mock_git

    local tree_sha
    printf 'multiline body\n' >blob-multi.txt
    tree_sha=$(write_tree_for_blob blob-multi.txt multi.txt)
    write_multiline_commit_file commit-multi.txt "$tree_sha"
    finalize_mock_commit commit-multi.txt
}

setup_empty_message_commit() {
    init_mock_git

    local tree_sha
    printf 'empty message\n' >blob-empty.txt
    tree_sha=$(write_tree_for_blob blob-empty.txt empty.txt)
    write_root_commit_file commit-empty.txt "$tree_sha" ""
    finalize_mock_commit commit-empty.txt
}

setup_message_commit() {
    local body="$1"
    local tree_sha

    init_mock_git
    printf 'msg\n' >blob-msg.txt
    tree_sha=$(write_tree_for_blob blob-msg.txt msg.txt)
    write_root_commit_file commit-msg.txt "$tree_sha" "$body"
    finalize_mock_commit commit-msg.txt
}

setup_different_author_committer_commit() {
    init_mock_git

    local tree_sha
    printf 'authored\n' >blob-author.txt
    tree_sha=$(write_tree_for_blob blob-author.txt author.txt)
    cat >commit-author.txt <<EOF
tree $tree_sha
author Alice Author <alice@example.com> 946684800 +0000
committer Bob Committer <bob@example.com> 946771200 -0700

different author and committer
EOF
    finalize_mock_commit commit-author.txt
}

setup_timezone_commit() {
    init_mock_git

    local tree_sha
    printf 'timezone\n' >blob-tz.txt
    tree_sha=$(write_tree_for_blob blob-tz.txt tz.txt)
    cat >commit-tz.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 -0530
committer Test User <test@example.com> 946857600 +0845

timezone offsets
EOF
    finalize_mock_commit commit-tz.txt
}

setup_utf8_commit() {
    init_mock_git

    local tree_sha
    printf 'utf8\n' >blob-utf8.txt
    tree_sha=$(write_tree_for_blob blob-utf8.txt utf8.txt)
    cat >commit-utf8.txt <<EOF
tree $tree_sha
author O'Brien <obrien@example.com> 946684800 +0000
committer O'Brien <obrien@example.com> 946684800 +0000

café résumé 日本語
EOF
    finalize_mock_commit commit-utf8.txt
}

setup_epoch_timestamp_commit() {
    init_mock_git

    local tree_sha
    printf 'epoch\n' >blob-epoch.txt
    tree_sha=$(write_tree_for_blob blob-epoch.txt epoch.txt)
    cat >commit-epoch.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 0 +0000
committer Test User <test@example.com> 0 +0000

epoch timestamp
EOF
    finalize_mock_commit commit-epoch.txt
}

setup_long_chain_commit() {
    init_mock_git

    local blob_sha tree_sha parent_sha child_sha i
    printf 'v0\n' >blob-v0.txt
    tree_sha=$(write_tree_for_blob blob-v0.txt v0.txt)
    cat >commit-v0.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

version 0
EOF
    parent_sha=$(git_write_commit commit-v0.txt)

    for i in 1 2 3 4 5; do
        printf 'v%s\n' "$i" >"blob-v$i.txt"
        tree_sha=$(write_tree_for_blob "blob-v$i.txt" "v$i.txt")
        cat >"commit-v$i.txt" <<EOF
tree $tree_sha
parent $parent_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

version $i
EOF
        parent_sha=$(git_write_commit "commit-v$i.txt")
    done

    sync_git_to_qgit
    echo "$parent_sha"
}

setup_octopus_merge_commit() {
    init_mock_git

    local blob_sha tree_sha root_sha sha_a sha_b sha_c merge_sha
    printf 'base\n' >blob-base.txt
    tree_sha=$(write_tree_for_blob blob-base.txt base.txt)
    cat >commit-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

root
EOF
    root_sha=$(git_write_commit commit-root.txt)

    printf 'branch-a\n' >blob-a.txt
    tree_sha=$(write_tree_for_blob blob-a.txt a.txt)
    cat >commit-a.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0000
committer Test User <test@example.com> 946771200 +0000

branch a
EOF
    sha_a=$(git_write_commit commit-a.txt)

    printf 'branch-b\n' >blob-b.txt
    tree_sha=$(write_tree_for_blob blob-b.txt b.txt)
    cat >commit-b.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946857600 +0000
committer Test User <test@example.com> 946857600 +0000

branch b
EOF
    sha_b=$(git_write_commit commit-b.txt)

    printf 'branch-c\n' >blob-c.txt
    tree_sha=$(write_tree_for_blob blob-c.txt c.txt)
    cat >commit-c.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946944000 +0000
committer Test User <test@example.com> 946944000 +0000

branch c
EOF
    sha_c=$(git_write_commit commit-c.txt)

    printf 'merged\n' >blob-merge.txt
    tree_sha=$(write_tree_for_blob blob-merge.txt merged.txt)
    cat >commit-merge.txt <<EOF
tree $tree_sha
parent $sha_a
parent $sha_b
parent $sha_c
author Test User <test@example.com> 947030400 +0000
committer Test User <test@example.com> 947030400 +0000

octopus merge
EOF
    merge_sha=$(git_write_commit commit-merge.txt)

    sync_git_to_qgit
    echo "$merge_sha"
}
