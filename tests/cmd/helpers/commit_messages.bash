write_root_commit_file() {
    local file="$1"
    local tree_sha="$2"
    local body="$3"

    cat >"$file" <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

$body
EOF
}

write_multiline_commit_file() {
    local file="$1"
    local tree_sha="$2"

    cat >"$file" <<EOF
tree $tree_sha
author Test User <test@example.com> 946684800 +0000
committer Test User <test@example.com> 946684800 +0000

line one
line two
line three
EOF
}
