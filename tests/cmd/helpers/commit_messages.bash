# Shared commit message fixtures (UTF-8 only, no encoding header).

write_root_commit_file() {
    local out_file="$1"
    local tree_sha="$2"
    local body="$3"

    {
        printf 'tree %s\n' "$tree_sha"
        printf 'author Test User <test@example.com> 946684800 +0000\n'
        printf 'committer Test User <test@example.com> 946684800 +0000\n'
        printf '\n'
        if [ -n "$body" ]; then
            printf '%s' "$body"
        fi
    } > "$out_file"
}

write_multiline_commit_file() {
    local out_file="$1"
    local tree_sha="$2"

    cat > "$out_file" <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

subject line

body paragraph one
body paragraph two
EOF
}
