assert_success() {
    if [ "$status" -ne 0 ]; then
        echo "Expected success but got status $status"
        return 1
    fi
}

assert_failure() {
    if [ "$status" -eq 0 ]; then
        echo "Expected failure but got status 0"
        return 1
    fi
}

assert_output_equals() {
    if [ "$output" != "$1" ]; then
        echo "Expected: $1"
        echo "Actual:   $output"
        return 1
    fi
}

assert_output_not_equals() {
    if [ "$output" == "$1" ]; then
        echo "Expected output to not be equal to: $1"
        echo "Actual output: $output"
        return 1
    fi
}

assert_output_empty() {
    if [ -n "$output" ]; then
        echo "Expected empty output but got: $output"
        return 1
    fi
}

assert_output_not_empty() {
    if [ -z "$output" ]; then
        echo "Expected non-empty output but got empty"
        return 1
    fi
}

assert_output_contains() {
    if [[ ! "$output" =~ "$1" ]]; then
        echo "Expected output to contain: $1"
        echo "Actual output: $output"
        return 1
    fi
}

assert_output_not_contains() {
    if [[ "$output" =~ "$1" ]]; then
        echo "Expected output to not contain: $1"
        echo "Actual output: $output"
        return 1
    fi
}

assert_output_matches() {
    local pattern="$1"
    if [[ ! "$output" =~ $pattern ]]; then
        echo "Expected output to match: $pattern"
        echo "Actual output: $output"
        return 1
    fi
}