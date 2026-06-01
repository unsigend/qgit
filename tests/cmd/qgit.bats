load 'helpers/globals'
load 'helpers/helpers'

@test "qgit" {
  run $QGIT_BIN
  assert_success
}

@test "qgit unknown" {
  run $QGIT_BIN unknown
  assert_failure
}

@test "qgit symbol" {
  run $QGIT_BIN ?
  assert_failure
}
