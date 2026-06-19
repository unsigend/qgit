load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"
load "helpers/commit_messages.bash"

# helpers

init_repo() {
    "$QGIT_BIN" init -q
}

setup_blob_from_file() {
    local file="$1"
    local hash

    init_mock_git
    hash=$(git_write_blob "$file")
    sync_git_to_qgit
    echo "$hash"
}

run_cat_file() {
    run "$QGIT_BIN" cat-file "$@"
}

git_cat() {
    env GIT_OBJECT_DIRECTORY="$TEST_DIR/.qgit/objects" GIT_DIR="$TEST_DIR/.qgit" "$GIT" cat-file "$@"
}

normalize_cat_output() {
    printf '%s' "$1" | tr -d ' \t\r'
}

assert_output_normalized_equals() {
    local expected="$1"
    local actual="$2"
    local norm_exp norm_act

    norm_exp="$(normalize_cat_output "$expected")"
    norm_act="$(normalize_cat_output "$actual")"
    if [ "$norm_exp" != "$norm_act" ]; then
        echo "Expected (normalized): $norm_exp"
        echo "Actual (normalized):   $norm_act"
        return 1
    fi
}

assert_matches_git_cat() {
    local expected
    expected=$(git_cat "$@")
    run_cat_file "$@"
    assert_success
    assert_output_equals "$expected"
}

assert_matches_git_cat_p() {
    local hash="$1"
    local expected

    expected=$(git_cat -p "$hash")
    run_cat_file -p "$hash"
    assert_success
    assert_output_normalized_equals "$expected" "$output"
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

write_ref() {
    local refname="$1"
    local sha="$2"
    local file

    file="$TEST_DIR/.qgit/$refname"
    mkdir -p "$(dirname "$file")"
    printf '%s\n' "$sha" >"$file"
}

set_symref_head() {
    local branch="$1"

    printf 'ref: refs/heads/%s\n' "$branch" >"$TEST_DIR/.qgit/HEAD"
}

setup_commit_with_branch_ref() {
    local sha

    sha=$(setup_root_commit)
    write_ref "refs/heads/$QGIT_DEFAULT_BRANCH" "$sha"
    set_symref_head "$QGIT_DEFAULT_BRANCH"
    printf '%s\n' "$sha"
}

abbrev_sha() {
    local sha="$1"
    local len="$2"

    printf '%s' "${sha:0:$len}"
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

# blob -p

@test "qgit cat-file -p: text blob matches git" {
    printf 'hello world\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: empty blob matches git" {
    : >empty
    local hash
    hash=$(setup_blob_from_file empty)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: binary blob matches git" {
    printf '\x00\x01\x02\xff\xfe' >binary
    local hash
    hash=$(setup_blob_from_file binary)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: large blob matches git" {
    dd if=/dev/zero bs=1024 count=64 of=large 2>/dev/null
    local hash
    hash=$(setup_blob_from_file large)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: utf8 blob matches git" {
    printf 'café 日本語\n' >utf8
    local hash
    hash=$(setup_blob_from_file utf8)
    assert_matches_git_cat -p "$hash"
}

@test "qgit cat-file -p: blob without trailing newline matches git" {
    printf 'no newline' >raw
    local hash
    hash=$(setup_blob_from_file raw)
    assert_matches_git_cat -p "$hash"
}

# blob -t

@test "qgit cat-file -t: blob type matches git" {
    printf 'typed blob\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: empty blob type matches git" {
    : >empty
    local hash
    hash=$(setup_blob_from_file empty)
    assert_matches_git_cat -t "$hash"
}

# blob -s

@test "qgit cat-file -s: blob size matches git" {
    printf 'size check\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: empty blob size matches git" {
    : >empty
    local hash
    hash=$(setup_blob_from_file empty)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: binary blob size matches git" {
    printf '\x00\x01\x02\xff\xfe' >binary
    local hash
    hash=$(setup_blob_from_file binary)
    assert_matches_git_cat -s "$hash"
}

# blob raw

@test "qgit cat-file blob: text blob matches git" {
    printf 'raw blob\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: empty blob matches git" {
    : >empty
    local hash
    hash=$(setup_blob_from_file empty)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: binary blob matches git" {
    printf '\x00\x01\x02\xff\xfe' >binary
    local hash
    hash=$(setup_blob_from_file binary)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: large blob matches git" {
    dd if=/dev/zero bs=1024 count=64 of=large 2>/dev/null
    local hash
    hash=$(setup_blob_from_file large)
    assert_matches_git_cat blob "$hash"
}

@test "qgit cat-file blob: wrong type fails" {
    printf 'blob only\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file commit "$hash"
    assert_failure
}

@test "qgit cat-file blob: tree type on blob fails" {
    printf 'blob only\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file tree "$hash"
    assert_failure
}

@test "qgit cat-file blob: tag type on blob fails" {
    printf 'blob only\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file tag "$hash"
    assert_failure
}

# commit -p

@test "qgit cat-file -p: root commit matches git" {
    local hash
    hash=$(setup_root_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: commit with parent matches git" {
    local hash
    hash=$(setup_commit_with_parent)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: merge commit matches git" {
    local hash
    hash=$(setup_merge_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: multiline commit message matches git" {
    local hash
    hash=$(setup_multiline_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: empty commit message matches git" {
    local hash
    hash=$(setup_empty_message_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: different author and committer matches git" {
    local hash
    hash=$(setup_different_author_committer_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: non-UTC timezone matches git" {
    local hash
    hash=$(setup_timezone_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: utf8 author and message matches git" {
    local hash
    hash=$(setup_utf8_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: epoch timestamp matches git" {
    local hash
    hash=$(setup_epoch_timestamp_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: long parent chain matches git" {
    local hash
    hash=$(setup_long_chain_commit)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: octopus merge matches git" {
    local hash
    hash=$(setup_octopus_merge_commit)
    assert_matches_git_cat_p "$hash"
}

# commit message edge cases

@test "qgit cat-file -p: message starting with author matches git" {
    local hash
    hash=$(setup_message_commit "author note only")
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: message starting with committer matches git" {
    local hash
    hash=$(setup_message_commit "committer note only")
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: message starting with tree matches git" {
    local hash
    hash=$(setup_message_commit "tree rebuild notes")
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: message starting with parent matches git" {
    local hash
    hash=$(setup_message_commit "parent linkage notes")
    assert_matches_git_cat_p "$hash"
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

@test "qgit cat-file -s: epoch timestamp commit size matches git" {
    local hash
    hash=$(setup_epoch_timestamp_commit)
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

@test "qgit cat-file commit: epoch timestamp matches git" {
    local hash
    hash=$(setup_epoch_timestamp_commit)
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

@test "qgit cat-file commit: tree type on commit fails" {
    local hash
    hash=$(setup_root_commit)
    run_cat_file tree "$hash"
    assert_failure
}

# tree -p

@test "qgit cat-file -p: single file tree matches git" {
    local hash
    hash=$(setup_single_file_tree)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: empty tree matches git" {
    local hash
    hash=$(setup_empty_tree)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: multi-entry tree matches git" {
    local hash
    hash=$(setup_multi_entry_tree)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: nested tree matches git" {
    local hash
    hash=$(setup_nested_tree)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: mixed file and subtree matches git" {
    local hash
    hash=$(setup_mixed_tree)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: executable entry tree matches git" {
    local hash
    hash=$(setup_executable_tree)
    assert_matches_git_cat_p "$hash"
}

@test "qgit cat-file -p: symlink entry tree matches git" {
    local hash
    hash=$(setup_symlink_tree)
    assert_matches_git_cat_p "$hash"
}

# tree -t

@test "qgit cat-file -t: single file tree type matches git" {
    local hash
    hash=$(setup_single_file_tree)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: empty tree type matches git" {
    local hash
    hash=$(setup_empty_tree)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: multi-entry tree type matches git" {
    local hash
    hash=$(setup_multi_entry_tree)
    assert_matches_git_cat -t "$hash"
}

@test "qgit cat-file -t: nested tree type matches git" {
    local hash
    hash=$(setup_nested_tree)
    assert_matches_git_cat -t "$hash"
}

# tree -s

@test "qgit cat-file -s: single file tree size matches git" {
    local hash
    hash=$(setup_single_file_tree)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: empty tree size matches git" {
    local hash
    hash=$(setup_empty_tree)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: multi-entry tree size matches git" {
    local hash
    hash=$(setup_multi_entry_tree)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: nested tree size matches git" {
    local hash
    hash=$(setup_nested_tree)
    assert_matches_git_cat -s "$hash"
}

@test "qgit cat-file -s: mixed file and subtree size matches git" {
    local hash
    hash=$(setup_mixed_tree)
    assert_matches_git_cat -s "$hash"
}

# tree raw

@test "qgit cat-file tree: single file tree matches git" {
    local hash
    hash=$(setup_single_file_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: empty tree matches git" {
    local hash
    hash=$(setup_empty_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: multi-entry tree matches git" {
    local hash
    hash=$(setup_multi_entry_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: nested tree matches git" {
    local hash
    hash=$(setup_nested_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: mixed file and subtree matches git" {
    local hash
    hash=$(setup_mixed_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: executable entry tree matches git" {
    local hash
    hash=$(setup_executable_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: symlink entry tree matches git" {
    local hash
    hash=$(setup_symlink_tree)
    assert_matches_git_cat tree "$hash"
}

@test "qgit cat-file tree: blob type on tree fails" {
    local hash
    hash=$(setup_single_file_tree)
    run_cat_file blob "$hash"
    assert_failure
}

@test "qgit cat-file tree: commit type on tree fails" {
    local hash
    hash=$(setup_single_file_tree)
    run_cat_file commit "$hash"
    assert_failure
}

@test "qgit cat-file tree: tag type on tree fails" {
    local hash
    hash=$(setup_single_file_tree)
    run_cat_file tag "$hash"
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
    local hash
    hash=$(setup_single_file_tree)
    run_cat_file -p -t "$hash"
    assert_failure
}

@test "qgit cat-file -p -s: flags cannot combine on tree" {
    local hash
    hash=$(setup_single_file_tree)
    run_cat_file -p -s "$hash"
    assert_failure
}

@test "qgit cat-file -s -t: flags cannot combine on tree" {
    local hash
    hash=$(setup_single_file_tree)
    run_cat_file -s -t "$hash"
    assert_failure
}

# name resolution

@test "qgit cat-file -t: HEAD resolves commit type" {
    setup_commit_with_branch_ref >/dev/null
    assert_matches_git_cat -t HEAD
}

@test "qgit cat-file -t: branch name resolves commit type" {
    setup_commit_with_branch_ref >/dev/null
    assert_matches_git_cat -t "$QGIT_DEFAULT_BRANCH"
}

@test "qgit cat-file -t: refs/heads path resolves commit type" {
    setup_commit_with_branch_ref >/dev/null
    assert_matches_git_cat -t "refs/heads/$QGIT_DEFAULT_BRANCH"
}

@test "qgit cat-file -t: short sha1 resolves commit type" {
    local sha abbrev

    sha=$(setup_commit_with_branch_ref)
    abbrev=$(abbrev_sha "$sha" 7)
    assert_matches_git_cat -t "$abbrev"
}

@test "qgit cat-file -p: HEAD resolves commit contents" {
    setup_commit_with_branch_ref >/dev/null
    assert_matches_git_cat_p HEAD
}

@test "qgit cat-file -t: tag name resolves commit type" {
    local sha

    sha=$(setup_commit_with_branch_ref)
    write_ref "refs/tags/v1.0" "$sha"
    assert_matches_git_cat -t v1.0
}

@test "qgit cat-file -p: unknown ref name fails" {
    setup_commit_with_branch_ref >/dev/null
    run_cat_file -p no-such-branch
    assert_failure
}

@test "qgit cat-file commit: HEAD resolves in raw mode" {
    setup_commit_with_branch_ref >/dev/null
    assert_matches_git_cat commit HEAD
}

@test "qgit cat-file commit: branch name resolves in raw mode" {
    setup_commit_with_branch_ref >/dev/null
    assert_matches_git_cat commit "$QGIT_DEFAULT_BRANCH"
}

@test "qgit cat-file commit: short sha1 resolves in raw mode" {
    local sha abbrev

    sha=$(setup_commit_with_branch_ref)
    abbrev=$(abbrev_sha "$sha" 7)
    assert_matches_git_cat commit "$abbrev"
}

@test "qgit cat-file blob: wrong type still fails with resolved ref name" {
    setup_commit_with_branch_ref >/dev/null
    run_cat_file blob HEAD
    assert_failure
}

# blob errors

@test "qgit cat-file -p: missing blob object file fails" {
    init_repo
    run_cat_file -p 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -t: missing blob object file fails" {
    init_repo
    run_cat_file -t 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -s: missing blob object file fails" {
    init_repo
    run_cat_file -s 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file blob: missing blob object file fails" {
    init_repo
    run_cat_file blob 0000000000000000000000000000000000000000
    assert_failure
}

@test "qgit cat-file -p -t: flags cannot combine on blob" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file -p -t "$hash"
    assert_failure
}

@test "qgit cat-file -p -s: flags cannot combine on blob" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file -p -s "$hash"
    assert_failure
}

@test "qgit cat-file -s -t: flags cannot combine on blob" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file -s -t "$hash"
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

@test "qgit cat-file commit: missing object fails" {
    init_repo
    run_cat_file commit
    assert_failure
}

@test "qgit cat-file tree: missing object fails" {
    init_repo
    run_cat_file tree
    assert_failure
}

@test "qgit cat-file: invalid type fails" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file invalid "$hash"
    assert_failure
}

@test "qgit cat-file: invalid object hash fails" {
    init_repo
    run_cat_file -p not-a-valid-sha
    assert_failure
}

@test "qgit cat-file: outside repository fails" {
    mkdir outside && cd outside || return 1
    run_cat_file -p deadbeef
    assert_failure
}

@test "qgit cat-file -p -t: flags cannot combine" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file -p -t "$hash"
    assert_failure
}

@test "qgit cat-file -p -s: flags cannot combine" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
    run_cat_file -p -s "$hash"
    assert_failure
}

@test "qgit cat-file -s -t: flags cannot combine" {
    printf 'data\n' >file
    local hash
    hash=$(setup_blob_from_file file)
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
