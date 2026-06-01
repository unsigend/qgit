load 'helpers/globals'
load 'helpers/helpers'

@test "version" {
  run $QGIT_BIN version
  assert_success
}

@test "version output" {
    run $QGIT_BIN version
    assert_output_matches "^qgit version [0-9]+\.[0-9]+\.[0-9]+$"
}