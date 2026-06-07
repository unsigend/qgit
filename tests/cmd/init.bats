load "helpers/setup.bash"
load "helpers/helpers.bash"
load "helpers/globals.bash"

@test "qgit init" {
    run "$QGIT_BIN" init
    assert_success
    assert_qgit_repo_layout
}

@test "qgit init: HEAD points to main" {
    run "$QGIT_BIN" init
    assert_success
    assert_file_content_contains ".qgit/HEAD" "ref: refs/heads/main"
}

@test "qgit init: config contains core settings" {
    run "$QGIT_BIN" init
    assert_success
    assert_file_content_contains ".qgit/config" "[core]"
    assert_file_content_contains ".qgit/config" "repositoryformatversion=0"
    assert_file_content_contains ".qgit/config" "filemode=false"
    assert_file_content_contains ".qgit/config" "bare=false"
}

@test "qgit init <path>: creates repo at path" {
    mkdir target
    run "$QGIT_BIN" init target
    assert_success
    assert_qgit_repo_layout target
    if [ -d .qgit ]; then
        echo "Expected no .qgit in cwd"
        return 1
    fi
}

@test "qgit init -b dev: HEAD points to dev" {
    run "$QGIT_BIN" init -b dev
    assert_success
    assert_file_content_contains ".qgit/HEAD" "ref: refs/heads/dev"
}

@test "qgit init --initial-branch: HEAD points to branch" {
    run "$QGIT_BIN" init --initial-branch feature
    assert_success
    assert_file_content_contains ".qgit/HEAD" "ref: refs/heads/feature"
}

@test "qgit init: prints initialized message" {
    run "$QGIT_BIN" init
    assert_success
    assert_output_matches "^Initialized empty qgit repository in .+"
}

@test "qgit init -q: no output" {
    run "$QGIT_BIN" init -q
    assert_success
    assert_output_empty
    assert_qgit_repo_layout
}

@test "qgit init: reinit existing repo" {
    run "$QGIT_BIN" init
    assert_success
    echo "custom marker" > .qgit/description

    run "$QGIT_BIN" init
    assert_success
    assert_output_contains "Reinitialized existing qgit repository in"
    assert_file_content_contains ".qgit/description" "custom marker"
}

@test "qgit init -b dev: does not overwrite HEAD on reinit" {
    run "$QGIT_BIN" init
    assert_success

    run "$QGIT_BIN" init -b dev
    assert_success
    assert_file_content_contains ".qgit/HEAD" "ref: refs/heads/main"
}

@test "qgit init -b \"\": invalid branch name" {
    run "$QGIT_BIN" init -b ""
    assert_failure
    assert_output_contains "invalid branch name"
}

@test "qgit init: fails when parent path cannot be created" {
    touch blocked
    run "$QGIT_BIN" init blocked/sub
    assert_failure
    rm -f blocked
}

@test "qgit init --unknown-flag: fails" {
    run "$QGIT_BIN" init --unknown-flag
    assert_failure
}

@test "qgit init: all entries exist and non-empty" {
    run "$QGIT_BIN" init
    assert_success
    assert_qgit_repo_layout
}

@test "qgit init: resolves relative path" {
    mkdir -p foo/bar
    run "$QGIT_BIN" init ./foo/../bar/repo
    assert_success
    assert_qgit_repo_layout bar/repo
}
