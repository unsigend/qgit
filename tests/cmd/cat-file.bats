load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"
load "helpers/commit_messages.bash"

# helpers

init_repo() {
    "$QGIT_BIN" init -q
}

write_blob() {
    "$QGIT_BIN" hash-object -w "$1"
}

run_cat_file() {
    run "$QGIT_BIN" cat-file "$@"
}

git_cat() {
    env GIT_OBJECT_DIRECTORY="$TEST_DIR/.qgit/objects" GIT_DIR="$TEST_DIR/.qgit" "$GIT" cat-file "$@"
}

assert_matches_git_cat() {
    local expected
    expected=$(git_cat "$@")
    run_cat_file "$@"
    assert_success
    assert_output_equals "$expected"
}

init_mock_git() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" init -q
}

sync_git_to_qgit() {
    cp -r "$TEST_DIR/.git" "$TEST_DIR/.qgit"
}

git_write_blob() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" hash-object -w "$@"
}

git_write_tree() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" mktree
}

git_write_commit() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" hash-object -w -t commit "$@"
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
    printf 'hello world\n' > blob-root.txt
    blob_sha=$(git_write_blob blob-root.txt)
    tree_sha=$(printf '100644 blob %s\tfile.txt\n' "$blob_sha" | git_write_tree)
    cat > commit-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

Root commit message
EOF
    commit_sha=$(git_write_commit commit-root.txt)

    sync_git_to_qgit
    echo "$commit_sha"
}

setup_commit_with_parent() {
    init_mock_git

    local blob_sha tree_sha root_sha child_sha
    printf 'base content\n' > blob-base.txt
    blob_sha=$(git_write_blob blob-base.txt)
    tree_sha=$(printf '100644 blob %s\tbase.txt\n' "$blob_sha" | git_write_tree)
    cat > commit-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

initial commit
EOF
    root_sha=$(git_write_commit commit-root.txt)

    printf 'child content\n' > blob-child.txt
    blob_sha=$(git_write_blob blob-child.txt)
    tree_sha=$(printf '100644 blob %s\tchild.txt\n' "$blob_sha" | git_write_tree)
    cat > commit-child.txt <<EOF
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
    printf 'base\n' > blob-base.txt
    blob_sha=$(git_write_blob blob-base.txt)
    tree_sha=$(printf '100644 blob %s\tbase.txt\n' "$blob_sha" | git_write_tree)
    cat > commit-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

root
EOF
    root_sha=$(git_write_commit commit-root.txt)

    printf 'branch-a\n' > blob-a.txt
    blob_sha=$(git_write_blob blob-a.txt)
    tree_sha=$(printf '100644 blob %s\ta.txt\n' "$blob_sha" | git_write_tree)
    cat > commit-a.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0000
committer Test User <test@example.com> 946771200 +0000

branch a
EOF
    commit_a_sha=$(git_write_commit commit-a.txt)

    printf 'branch-b\n' > blob-b.txt
    blob_sha=$(git_write_blob blob-b.txt)
    tree_sha=$(printf '100644 blob %s\tb.txt\n' "$blob_sha" | git_write_tree)
    cat > commit-b.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946857600 +0000
committer Test User <test@example.com> 946857600 +0000

branch b
EOF
    commit_b_sha=$(git_write_commit commit-b.txt)

    printf 'merged\n' > blob-merge.txt
    blob_sha=$(git_write_blob blob-merge.txt)
    tree_sha=$(printf '100644 blob %s\tmerged.txt\n' "$blob_sha" | git_write_tree)
    cat > commit-merge.txt <<EOF
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
    printf 'multiline body\n' > blob-multi.txt
    tree_sha=$(write_tree_for_blob blob-multi.txt multi.txt)
    write_multiline_commit_file commit-multi.txt "$tree_sha"
    finalize_mock_commit commit-multi.txt
}

setup_empty_message_commit() {
    init_mock_git

    local tree_sha
    printf 'empty message\n' > blob-empty.txt
    tree_sha=$(write_tree_for_blob blob-empty.txt empty.txt)
    write_root_commit_file commit-empty.txt "$tree_sha" ""
    finalize_mock_commit commit-empty.txt
}

setup_message_commit() {
    local body="$1"
    local tree_sha

    init_mock_git
    printf 'msg\n' > blob-msg.txt
    tree_sha=$(write_tree_for_blob blob-msg.txt msg.txt)
    write_root_commit_file commit-msg.txt "$tree_sha" "$body"
    finalize_mock_commit commit-msg.txt
}

setup_different_author_committer_commit() {
    init_mock_git

    local tree_sha
    printf 'authored\n' > blob-author.txt
    tree_sha=$(write_tree_for_blob blob-author.txt author.txt)
    cat > commit-author.txt <<EOF
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
    printf 'timezone\n' > blob-tz.txt
    tree_sha=$(write_tree_for_blob blob-tz.txt tz.txt)
    cat > commit-tz.txt <<EOF
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
    printf 'utf8\n' > blob-utf8.txt
    tree_sha=$(write_tree_for_blob blob-utf8.txt utf8.txt)
    cat > commit-utf8.txt <<EOF
tree $tree_sha
author O'Brien <obrien@example.com> 946684800 +0000
committer O'Brien <obrien@example.com> 946684800 +0000

café résumé 日本語
EOF
    finalize_mock_commit commit-utf8.txt
}

setup_long_chain_commit() {
    init_mock_git

    local blob_sha tree_sha parent_sha child_sha i
    printf 'v0\n' > blob-v0.txt
    tree_sha=$(write_tree_for_blob blob-v0.txt v0.txt)
    cat > commit-v0.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

version 0
EOF
    parent_sha=$(git_write_commit commit-v0.txt)

    for i in 1 2 3 4 5; do
        printf 'v%s\n' "$i" > "blob-v$i.txt"
        tree_sha=$(write_tree_for_blob "blob-v$i.txt" "v$i.txt")
        cat > "commit-v$i.txt" <<EOF
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
    printf 'base\n' > blob-base.txt
    tree_sha=$(write_tree_for_blob blob-base.txt base.txt)
    cat > commit-root.txt <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

root
EOF
    root_sha=$(git_write_commit commit-root.txt)

    printf 'branch-a\n' > blob-a.txt
    tree_sha=$(write_tree_for_blob blob-a.txt a.txt)
    cat > commit-a.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946771200 +0000
committer Test User <test@example.com> 946771200 +0000

branch a
EOF
    sha_a=$(git_write_commit commit-a.txt)

    printf 'branch-b\n' > blob-b.txt
    tree_sha=$(write_tree_for_blob blob-b.txt b.txt)
    cat > commit-b.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946857600 +0000
committer Test User <test@example.com> 946857600 +0000

branch b
EOF
    sha_b=$(git_write_commit commit-b.txt)

    printf 'branch-c\n' > blob-c.txt
    tree_sha=$(write_tree_for_blob blob-c.txt c.txt)
    cat > commit-c.txt <<EOF
tree $tree_sha
parent $root_sha
author Test User <test@example.com> 946944000 +0000
committer Test User <test@example.com> 946944000 +0000

branch c
EOF
    sha_c=$(git_write_commit commit-c.txt)

    printf 'merged\n' > blob-merge.txt
    tree_sha=$(write_tree_for_blob blob-merge.txt merged.txt)
    cat > commit-merge.txt <<EOF
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

CAT_FILE_EMPTY_TREE="4b825dc642cb6eb9a060e54bf8d69288fbee4904"

commit_tree_sha() {
    env GIT_DIR="$TEST_DIR/.git" "$GIT" rev-parse "$1^{tree}"
}

setup_single_file_tree() {
    local tree_sha

    init_mock_git
    printf 'hello\n' > blob-single.txt
    tree_sha=$(write_tree_for_blob blob-single.txt single.txt)
    sync_git_to_qgit
    echo "$tree_sha"
}

setup_multi_entry_tree() {
    local blob_a blob_b blob_c tree_sha

    init_mock_git
    printf 'alpha\n' > blob-alpha.txt
    printf 'beta\n' > blob-beta.txt
    printf 'gamma\n' > blob-gamma.txt
    blob_a=$(git_write_blob blob-alpha.txt)
    blob_b=$(git_write_blob blob-beta.txt)
    blob_c=$(git_write_blob blob-gamma.txt)
    tree_sha=$(
        {
            printf '100644 blob %s\talpha.txt\n' "$blob_a"
            printf '100644 blob %s\tbeta.txt\n' "$blob_b"
            printf '100644 blob %s\tgamma.txt\n' "$blob_c"
        } | git_write_tree
    )
    sync_git_to_qgit
    echo "$tree_sha"
}

setup_nested_tree() {
    local inner_blob inner_tree root_blob tree_sha

    init_mock_git
    printf 'inner\n' > blob-inner.txt
    inner_blob=$(git_write_blob blob-inner.txt)
    inner_tree=$(printf '100644 blob %s\tinner.txt\n' "$inner_blob" | git_write_tree)

    printf 'root\n' > blob-root.txt
    root_blob=$(git_write_blob blob-root.txt)
    tree_sha=$(
        {
            printf '100644 blob %s\troot.txt\n' "$root_blob"
            printf '040000 tree %s\tsubdir\n' "$inner_tree"
        } | git_write_tree
    )
    sync_git_to_qgit
    echo "$tree_sha"
}

setup_mixed_mode_tree() {
    local blob_reg blob_exec blob_link inner_tree tree_sha

    init_mock_git
    printf 'regular\n' > blob-regular.txt
    printf 'executable\n' > blob-executable.txt
    blob_reg=$(git_write_blob blob-regular.txt)
    blob_exec=$(git_write_blob blob-executable.txt)
    blob_link=$(git_write_blob blob-regular.txt)
    inner_tree=$(printf '100644 blob %s\tinner.txt\n' "$blob_reg" | git_write_tree)

    tree_sha=$(
        {
            printf '100644 blob %s\tregular.txt\n' "$blob_reg"
            printf '100755 blob %s\texecutable.sh\n' "$blob_exec"
            printf '120000 blob %s\tlink.txt\n' "$blob_link"
            printf '040000 tree %s\tdirectory\n' "$inner_tree"
        } | git_write_tree
    )
    sync_git_to_qgit
    echo "$tree_sha"
}

setup_empty_tree() {
    init_mock_git
    env GIT_DIR="$TEST_DIR/.git" "$GIT" mktree < /dev/null >/dev/null
    sync_git_to_qgit
    echo "$CAT_FILE_EMPTY_TREE"
}

# blob -p

@test "qgit cat-file -p: text blob matches git" {
    init_repo
    printf 'hello world\n' > file
    local hash
    hash=$(write_blob file)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: empty blob matches git" {
    init_repo
    : > empty
    local hash
    hash=$(write_blob empty)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: binary blob matches git" {
    init_repo
    printf '\x00\x01\x02\xff\xfe' > binary
    local hash
    hash=$(write_blob binary)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: large blob matches git" {
    init_repo
    dd if=/dev/zero bs=1024 count=64 of=large 2>/dev/null
    local hash
    hash=$(write_blob large)
    assert_matches_git_cat -p "$hash"
}

# blob -t

@test "qgit cat-file -t: blob type matches git" {
    init_repo
    printf 'typed blob\n' > file
    local hash
    hash=$(write_blob file)
    assert_matches_git_cat -t "$hash"
}

# blob -s

@test "qgit cat-file -s: blob size matches git" {
    init_repo
    printf 'size check\n' > file
    local hash
    hash=$(write_blob file)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: empty blob size matches git" {
    init_repo
    : > empty
    local hash
    hash=$(write_blob empty)
    assert_matches_git_cat -s "$hash"
}

# blob raw

@test "qgit cat-file blob: text blob matches git" {
    init_repo
    printf 'raw blob\n' > file
    local hash
    hash=$(write_blob file)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: empty blob matches git" {
    init_repo
    : > empty
    local hash
    hash=$(write_blob empty)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: binary blob matches git" {
    init_repo
    printf '\x00\x01\x02\xff\xfe' > binary
    local hash
    hash=$(write_blob binary)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: wrong type fails" {
    init_repo
    printf 'blob only\n' > file
    local hash
    hash=$(write_blob file)
    run_cat_file commit "$hash"
    assert_failure
}

@test "qgit cat-file blob: tree type on blob fails" {
    init_repo
    printf 'blob only\n' > file
    local hash
    hash=$(write_blob file)
    run_cat_file tree "$hash"
    assert_failure
}

# commit -p

@test "qgit cat-file -p: root commit matches git" {
    local hash
    hash=$(setup_root_commit)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: commit with parent matches git" {
    local hash
    hash=$(setup_commit_with_parent)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: merge commit matches git" {
    local hash
    hash=$(setup_merge_commit)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: multiline commit message matches git" {
    local hash
    hash=$(setup_multiline_commit)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: empty commit message matches git" {
    local hash
    hash=$(setup_empty_message_commit)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: different author and committer matches git" {
    local hash
    hash=$(setup_different_author_committer_commit)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: non-UTC timezone matches git" {
    local hash
    hash=$(setup_timezone_commit)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: utf8 author and message matches git" {
    local hash
    hash=$(setup_utf8_commit)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: long parent chain matches git" {
    local hash
    hash=$(setup_long_chain_commit)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: octopus merge matches git" {
    local hash
    hash=$(setup_octopus_merge_commit)
    assert_matches_git_cat -p "$hash"
}

# commit message edge cases

@test "qgit cat-file -p: message starting with author matches git" {
    local hash
    hash=$(setup_message_commit "author note only")
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: message starting with committer matches git" {
    local hash
    hash=$(setup_message_commit "committer note only")
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: message starting with tree matches git" {
    local hash
    hash=$(setup_message_commit "tree rebuild notes")
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: message starting with parent matches git" {
    local hash
    hash=$(setup_message_commit "parent linkage notes")
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file commit: message starting with tree matches git" {
    local hash
    hash=$(setup_message_commit "tree rebuild notes")
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: message starting with parent matches git" {
    local hash
    hash=$(setup_message_commit "parent linkage notes")
    assert_matches_git_cat commit "$hash"
}

# commit -t

@test "qgit cat-file -t: root commit type matches git" {
    local hash
    hash=$(setup_root_commit)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: merge commit type matches git" {
    local hash
    hash=$(setup_merge_commit)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: commit with parent type matches git" {
    local hash
    hash=$(setup_commit_with_parent)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: empty message commit type matches git" {
    local hash
    hash=$(setup_empty_message_commit)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: octopus merge type matches git" {
    local hash
    hash=$(setup_octopus_merge_commit)
    assert_matches_git_cat -t "$hash"
}

# commit -s

@test "qgit cat-file -s: root commit size matches git" {
    local hash
    hash=$(setup_root_commit)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: commit with parent size matches git" {
    local hash
    hash=$(setup_commit_with_parent)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: merge commit size matches git" {
    local hash
    hash=$(setup_merge_commit)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: empty message commit size matches git" {
    local hash
    hash=$(setup_empty_message_commit)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: multiline commit size matches git" {
    local hash
    hash=$(setup_multiline_commit)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: octopus merge size matches git" {
    local hash
    hash=$(setup_octopus_merge_commit)
    assert_matches_git_cat -s "$hash"
}

# commit raw

@test "qgit cat-file commit: root commit matches git" {
    local hash
    hash=$(setup_root_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: commit with parent matches git" {
    local hash
    hash=$(setup_commit_with_parent)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: merge commit matches git" {
    local hash
    hash=$(setup_merge_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: multiline commit message matches git" {
    local hash
    hash=$(setup_multiline_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: empty message matches git" {
    local hash
    hash=$(setup_empty_message_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: different author and committer matches git" {
    local hash
    hash=$(setup_different_author_committer_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: long parent chain matches git" {
    local hash
    hash=$(setup_long_chain_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: octopus merge matches git" {
    local hash
    hash=$(setup_octopus_merge_commit)
    assert_matches_git_cat commit "$hash"
}

@test "qgit cat-file commit: blob type on commit fails" {
    local hash
    hash=$(setup_root_commit)
    run_cat_file blob "$hash"
    assert_failure
}

@test "qgit cat-file commit: tag type on commit fails" {
    local hash
    hash=$(setup_root_commit)
    run_cat_file tag "$hash"
    assert_failure
}

# tree -p

@test "qgit cat-file -p: single-entry tree matches git" {
    local tree_sha
    tree_sha=$(setup_single_file_tree)
    assert_matches_git_cat -p "$tree_sha"
}

@test "qgit cat-file -p: multi-entry tree matches git" {
    local tree_sha
    tree_sha=$(setup_multi_entry_tree)
    assert_matches_git_cat -p "$tree_sha"
}

@test "qgit cat-file -p: nested tree lists top level only" {
    local tree_sha
    tree_sha=$(setup_nested_tree)
    assert_matches_git_cat -p "$tree_sha"
}

@test "qgit cat-file -p: mixed mode tree matches git" {
    local tree_sha
    tree_sha=$(setup_mixed_mode_tree)
    assert_matches_git_cat -p "$tree_sha"
}

@test "qgit cat-file -p: empty tree matches git" {
    local tree_sha
    tree_sha=$(setup_empty_tree)
    assert_matches_git_cat -p "$tree_sha"
}

@test "qgit cat-file -p: tree from commit tip matches git" {
    local commit_sha tree_sha
    commit_sha=$(setup_root_commit)
    tree_sha=$(commit_tree_sha "$commit_sha")
    assert_matches_git_cat -p "$tree_sha"
}

@test "qgit cat-file -p: commit sha shows commit not tree" {
    local commit_sha tree_sha
    commit_sha=$(setup_root_commit)
    tree_sha=$(commit_tree_sha "$commit_sha")
    assert_matches_git_cat -p "$commit_sha"
    run_cat_file -p "$commit_sha"
    assert_success
    assert_output_not_equals "$(git_cat -p "$tree_sha")"
}

# tree -t

@test "qgit cat-file -t: single-entry tree type matches git" {
    local tree_sha
    tree_sha=$(setup_single_file_tree)
    assert_matches_git_cat -t "$tree_sha"
}

@test "qgit cat-file -t: multi-entry tree type matches git" {
    local tree_sha
    tree_sha=$(setup_multi_entry_tree)
    assert_matches_git_cat -t "$tree_sha"
}

@test "qgit cat-file -t: nested tree type matches git" {
    local tree_sha
    tree_sha=$(setup_nested_tree)
    assert_matches_git_cat -t "$tree_sha"
}

@test "qgit cat-file -t: mixed mode tree type matches git" {
    local tree_sha
    tree_sha=$(setup_mixed_mode_tree)
    assert_matches_git_cat -t "$tree_sha"
}

@test "qgit cat-file -t: empty tree type matches git" {
    local tree_sha
    tree_sha=$(setup_empty_tree)
    assert_matches_git_cat -t "$tree_sha"
}

# tree -s

@test "qgit cat-file -s: single-entry tree size matches git" {
    local tree_sha
    tree_sha=$(setup_single_file_tree)
    assert_matches_git_cat -s "$tree_sha"
}

@test "qgit cat-file -s: multi-entry tree size matches git" {
    local tree_sha
    tree_sha=$(setup_multi_entry_tree)
    assert_matches_git_cat -s "$tree_sha"
}

@test "qgit cat-file -s: nested tree size matches git" {
    local tree_sha
    tree_sha=$(setup_nested_tree)
    assert_matches_git_cat -s "$tree_sha"
}

@test "qgit cat-file -s: mixed mode tree size matches git" {
    local tree_sha
    tree_sha=$(setup_mixed_mode_tree)
    assert_matches_git_cat -s "$tree_sha"
}

@test "qgit cat-file -s: empty tree size matches git" {
    local tree_sha
    tree_sha=$(setup_empty_tree)
    assert_matches_git_cat -s "$tree_sha"
}

# tree raw

@test "qgit cat-file tree: single-entry tree matches git" {
    local tree_sha
    tree_sha=$(setup_single_file_tree)
    assert_matches_git_cat tree "$tree_sha"
}

@test "qgit cat-file tree: multi-entry tree matches git" {
    local tree_sha
    tree_sha=$(setup_multi_entry_tree)
    assert_matches_git_cat tree "$tree_sha"
}

@test "qgit cat-file tree: nested tree matches git" {
    local tree_sha
    tree_sha=$(setup_nested_tree)
    assert_matches_git_cat tree "$tree_sha"
}

@test "qgit cat-file tree: mixed mode tree matches git" {
    local tree_sha
    tree_sha=$(setup_mixed_mode_tree)
    assert_matches_git_cat tree "$tree_sha"
}

@test "qgit cat-file tree: empty tree matches git" {
    local tree_sha
    tree_sha=$(setup_empty_tree)
    assert_matches_git_cat tree "$tree_sha"
}

@test "qgit cat-file tree: blob type on tree fails" {
    local tree_sha blob_sha
    tree_sha=$(setup_single_file_tree)
    blob_sha=$(git_write_blob blob-single.txt)
    run_cat_file blob "$tree_sha"
    assert_failure
}

@test "qgit cat-file tree: commit type on tree fails" {
    local tree_sha commit_sha
    commit_sha=$(setup_root_commit)
    tree_sha=$(commit_tree_sha "$commit_sha")
    run_cat_file commit "$tree_sha"
    assert_failure
}

@test "qgit cat-file tree: tag type on tree fails" {
    local tree_sha
    tree_sha=$(setup_single_file_tree)
    run_cat_file tag "$tree_sha"
    assert_failure
}

# tree vs git

@test "qgit cat-file tree: commit sha fails without peeling to tree" {
    local commit_sha
    commit_sha=$(setup_root_commit)
    git_cat tree "$commit_sha" >/dev/null || return 1
    run_cat_file tree "$commit_sha"
    assert_failure
}

@test "qgit cat-file tree: merge commit sha fails without peeling to tree" {
    local commit_sha
    commit_sha=$(setup_merge_commit)
    git_cat tree "$commit_sha" >/dev/null || return 1
    run_cat_file tree "$commit_sha"
    assert_failure
}

# tree errors

@test "qgit cat-file -p: missing tree object file fails" {
    setup_single_file_tree >/dev/null
    run_cat_file -p 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -t: missing tree object file fails" {
    setup_single_file_tree >/dev/null
    run_cat_file -t 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -s: missing tree object file fails" {
    setup_single_file_tree >/dev/null
    run_cat_file -s 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file tree: missing tree object file fails" {
    setup_single_file_tree >/dev/null
    run_cat_file tree 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -p -t: flags cannot combine on tree" {
    local tree_sha
    tree_sha=$(setup_single_file_tree)
    run_cat_file -p -t "$tree_sha"
    assert_failure
}

@test "qgit cat-file -p -s: flags cannot combine on tree" {
    local tree_sha
    tree_sha=$(setup_single_file_tree)
    run_cat_file -p -s "$tree_sha"
    assert_failure
}

@test "qgit cat-file -s -t: flags cannot combine on tree" {
    local tree_sha
    tree_sha=$(setup_single_file_tree)
    run_cat_file -s -t "$tree_sha"
    assert_failure
}

# commit errors

@test "qgit cat-file -p: missing commit object file fails" {
    setup_root_commit >/dev/null
    run_cat_file -p 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -t: missing commit object file fails" {
    setup_root_commit >/dev/null
    run_cat_file -t 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -s: missing commit object file fails" {
    setup_root_commit >/dev/null
    run_cat_file -s 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file commit: missing commit object file fails" {
    setup_root_commit >/dev/null
    run_cat_file commit 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -p -t: flags cannot combine on commit" {
    local hash
    hash=$(setup_root_commit)
    run_cat_file -p -t "$hash"
    assert_failure
}

@test "qgit cat-file -p -s: flags cannot combine on commit" {
    local hash
    hash=$(setup_root_commit)
    run_cat_file -p -s "$hash"
    assert_failure
}

@test "qgit cat-file -s -t: flags cannot combine on commit" {
    local hash
    hash=$(setup_root_commit)
    run_cat_file -s -t "$hash"
    assert_failure
}

# errors

@test "qgit cat-file -p: missing object fails" {
    init_repo
    run_cat_file -p
    assert_failure
}

@test "qgit cat-file -t: missing object fails" {
    init_repo
    run_cat_file -t
    assert_failure
}

@test "qgit cat-file -s: missing object fails" {
    init_repo
    run_cat_file -s
    assert_failure
}

@test "qgit cat-file blob: missing object fails" {
    init_repo
    run_cat_file blob
    assert_failure
}

@test "qgit cat-file: invalid type fails" {
    init_repo
    printf 'data\n' > file
    local hash
    hash=$(write_blob file)
    run_cat_file invalid "$hash"
    assert_failure
}

@test "qgit cat-file -p: missing object file fails" {
    init_repo
    run_cat_file -p 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file blob: missing object file fails" {
    init_repo
    run_cat_file blob 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file: outside repository fails" {
    mkdir outside && cd outside || return 1
    run_cat_file -p deadbeef
    assert_failure
}

@test "qgit cat-file -p -t: flags cannot combine" {
    init_repo
    printf 'data\n' > file
    local hash
    hash=$(write_blob file)
    run_cat_file -p -t "$hash"
    assert_failure
}

@test "qgit cat-file -p -s: flags cannot combine" {
    init_repo
    printf 'data\n' > file
    local hash
    hash=$(write_blob file)
    run_cat_file -p -s "$hash"
    assert_failure
}

@test "qgit cat-file -s -t: flags cannot combine" {
    init_repo
    printf 'data\n' > file
    local hash
    hash=$(write_blob file)
    run_cat_file -s -t "$hash"
    assert_failure
}

# -h

@test "qgit cat-file -h: shows help" {
    run_cat_file -h
    assert_success
    assert_output_contains "qgit cat-file"
    assert_output_contains "-p"
}

@test "qgit cat-file --help: shows help" {
    run_cat_file --help
    assert_success
    assert_output_contains "qgit cat-file"
    assert_output_contains "-s"
}
