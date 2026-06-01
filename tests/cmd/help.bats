load 'helpers/globals'
load 'helpers/helpers'

@test "help" {
  run $QGIT_BIN help
  assert_success
}

@test "help output" {
    run $QGIT_BIN help
    assert_output_not_empty
}