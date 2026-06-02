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

assert_file_exists() {
    local file="$1"
    if [ ! -f "$file" ]; then
        echo "Expected file to exist: $file"
        return 1
    fi
}

assert_dir_exists() {
    local dir="$1"
    if [ ! -d "$dir" ]; then
        echo "Expected directory to exist: $dir"
        return 1
    fi
}

assert_file_content_equals() {
    local file="$1"
    local content="$2"
    if [ "$(cat "$file")" != "$content" ]; then
        echo "Expected file content to be: $content"
        echo "Actual file content: $(cat "$file")"
        return 1
    fi
}

assert_file_content_contains() {
    local file="$1"
    local content="$2"
    if [[ ! "$(cat "$file")" =~ "$content" ]]; then
        echo "Expected file content to contain: $content"
        echo "Actual file content: $(cat "$file")"
        return 1
    fi
}