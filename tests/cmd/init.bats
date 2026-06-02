load 'helpers/globals'
load 'helpers/helpers'
load 'helpers/setup'

@test "init status" {
  run $QGIT_BIN init
  assert_success
}

@test "init output" {
  run $QGIT_BIN init
  assert_output_not_empty
  assert_output_contains "Initialized empty qgit repository in"
}

@test "init create file" {
  run $QGIT_BIN init
  assert_success
  assert_dir_exists ".qgit"
  assert_dir_exists ".qgit/objects"
  assert_dir_exists ".qgit/refs/heads"
  assert_dir_exists ".qgit/refs/tags"
  assert_file_exists ".qgit/HEAD"
  assert_file_exists ".qgit/description"
  assert_file_exists ".qgit/config"
}

@test "init HEAD" {
  run $QGIT_BIN init
  assert_success
  assert_file_content_contains ".qgit/HEAD" "ref: refs/heads/main"
}

@test "init description" {
  run $QGIT_BIN init
  assert_success
  assert_file_content_contains ".qgit/description" "Unnamed repository"
}

@test "init config" {
  run $QGIT_BIN init
  assert_success
  assert_file_content_contains ".qgit/config" "[core]"
  assert_file_content_contains ".qgit/config" "repositoryformatversion"
  assert_file_content_contains ".qgit/config" "filemode"
  assert_file_content_contains ".qgit/config" "bare"
}

@test "init quiet" {
  run $QGIT_BIN init -q
  assert_success
  assert_output_empty
  assert_dir_exists ".qgit"
}

@test "init quiet reinit" {
  run $QGIT_BIN init
  run $QGIT_BIN init -q
  assert_success
  assert_output_empty
}

@test "init branch" {
  run $QGIT_BIN init -b develop
  assert_success
  assert_file_content_contains ".qgit/HEAD" "refs/heads/develop"
}

@test "init initial-branch" {
  run $QGIT_BIN init --initial-branch feature
  assert_success
  assert_file_content_contains ".qgit/HEAD" "refs/heads/feature"
}

@test "init path dot" {
  run $QGIT_BIN init .
  assert_success
  assert_dir_exists ".qgit"
  assert_output_contains "Initialized empty qgit repository in"
}

@test "init nested directory" {
  run $QGIT_BIN init nested
  assert_success
  assert_dir_exists "nested"
  assert_dir_exists "nested/.qgit"
  assert_output_contains "Initialized empty qgit repository in"
}

@test "init nested branch" {
  run $QGIT_BIN init -b topic nested
  assert_success
  assert_file_content_contains "nested/.qgit/HEAD" "refs/heads/topic"
}

@test "init nested quiet" {
  run $QGIT_BIN init -q nested
  assert_success
  assert_output_empty
  assert_dir_exists "nested/.qgit"
}

@test "init nested subdirectory" {
  run $QGIT_BIN init nested/repo
  assert_success
  assert_dir_exists "nested"
  assert_dir_exists "nested/repo/.qgit"
}

@test "init creates parent directories" {
  run $QGIT_BIN init missing/nested
  assert_success
  assert_dir_exists "missing"
  assert_dir_exists "missing/nested/.qgit"
}

@test "init deep nested path" {
  run $QGIT_BIN init a/b/c/repo
  assert_success
  assert_dir_exists "a/b/c/repo/.qgit"
}

@test "init reinit" {
  run $QGIT_BIN init
  assert_success
  run $QGIT_BIN init
  assert_success
  assert_output_contains "Reinitialized existing qgit repository in"
}

@test "init reinit preserves HEAD" {
  run $QGIT_BIN init -b develop
  run $QGIT_BIN init -b main
  assert_success
  assert_file_content_contains ".qgit/HEAD" "refs/heads/develop"
}

@test "init reinit quiet" {
  run $QGIT_BIN init
  run $QGIT_BIN init -q
  assert_success
  assert_output_empty
}

@test "init help" {
  run $QGIT_BIN init --help
  assert_success
  assert_output_contains "initial-branch"
  assert_output_contains "quiet"
}
