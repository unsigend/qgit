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

assert_file_content_not_contains() {
    local file="$1"
    local content="$2"
    if [[ "$(cat "$file")" =~ "$content" ]]; then
        echo "Expected file content to not contain: $content"
        echo "Actual file content: $(cat "$file")"
        return 1
    fi
}

qgit_meta_dir() {
    local root="${1:-.}"
    if [ "$root" = "." ]; then
        printf '%s\n' "$QGIT_META_DIR"
    else
        printf '%s/%s\n' "$root" "$QGIT_META_DIR"
    fi
}

assert_qgit_absent() {
    local root="${1:-.}"
    local meta
    meta=$(qgit_meta_dir "$root")
    if [ -e "$meta" ]; then
        echo "Expected no qgit meta dir: $meta"
        return 1
    fi
}

assert_qgit_head() {
    local branch="$1"
    local root="${2:-.}"
    assert_file_content_contains "$(qgit_meta_dir "$root")/HEAD" \
        "ref: refs/heads/$branch"
}

assert_qgit_config_defaults() {
    local root="${1:-.}"
    local config
    config="$(qgit_meta_dir "$root")/config"
    assert_file_content_contains "$config" "$QGIT_CONFIG_SECTION"
    local entry
    for entry in "${QGIT_CONFIG_ENTRIES[@]}"; do
        assert_file_content_contains "$config" "$entry"
    done
}

assert_qgit_description() {
    local root="${1:-.}"
    assert_file_content_contains "$(qgit_meta_dir "$root")/description" \
        "$QGIT_DESCRIPTION_LINE"
}

assert_qgit_repo_layout() {
    local root="${1:-.}"
    local meta
    meta=$(qgit_meta_dir "$root")

    assert_dir_exists "$meta"

    local entry
    for entry in "${QGIT_REPO_DIRS[@]}"; do
        assert_dir_exists "$meta/$entry"
    done
    for entry in "${QGIT_REPO_FILES[@]}"; do
        assert_file_exists "$meta/$entry"
        [ -s "$meta/$entry" ] || {
            echo "Expected non-empty $meta/$entry"
            return 1
        }
    done
}

# Normalize log output for rough comparison with git: collapse whitespace runs,
# trim each line, and drop blank lines. Newline count and spacing may differ.
normalize_log_output() {
    local text="$1"
    local line normalized
    while IFS= read -r line || [ -n "$line" ]; do
        normalized=$(printf '%s' "$line" | tr -s '[:space:]' ' ')
        normalized="${normalized# }"
        normalized="${normalized% }"
        if [ -n "$normalized" ]; then
            printf '%s\n' "$normalized"
        fi
    done <<< "$text"
}

assert_output_matches_normalized() {
    local expected="$1"
    local expected_norm actual_norm
    expected_norm=$(normalize_log_output "$expected")
    actual_norm=$(normalize_log_output "$output")
    if [ "$expected_norm" != "$actual_norm" ]; then
        echo "Expected (normalized):"
        echo "$expected_norm"
        echo "Actual (normalized):"
        echo "$actual_norm"
        return 1
    fi
}