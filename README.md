# qgit

qgit is a simplified Git like version control system written in C. It is rebuilt based on Git v1.0.0 with a reduced implementation aimed at exploring Git core internals. It supports common repository workflows and stays compatible with Git repositories.

## Table of Contents

1. [Build](#build)
2. [Commands](#commands)
   * [add](#add)
   * [cat-file](#cat-file)
   * [check-ignore](#check-ignore)
   * [checkout](#checkout)
   * [commit](#commit)
   * [config](#config)
   * [hash-object](#hash-object)
   * [help](#help)
   * [init](#init)
   * [log](#log)
   * [ls-files](#ls-files)
   * [ls-tree](#ls-tree)
   * [rev-parse](#rev-parse)
   * [rm](#rm)
   * [show-ref](#show-ref)
   * [status](#status)
   * [tag](#tag)
   * [version](#version)
3. [Contribution](#contribution)

## Build

Requires a C11 compiler, OpenSSL, and zlib.

```bash
make bin
```

The executable is placed at `build/bin/qgit`.

Available make targets:

```
make all       build executable and library
make bin       build executable
make lib       build library
make clean     clean build directory
make list      list source files
make info      show build configuration
make clang     generate compile_commands.json
make format    format .c and .h
make flags     show compiler flags
make docker    build and run development container
make install   install executable globally
make test      run all tests
make test-NAME run one test
make unit      build and run unit tests
make unit-NAME run one unit test
make help      show usage message
```

## Commands

Command documentation is in progress. Each entry below matches Git usage at a high level.

### add

Add file contents to the index.

---

### cat-file

Provide contents or details of repository objects.

qgit cat-file works in two modes.

#### Synopsis

```
qgit cat-file (-p | -t | -s) <object>
qgit cat-file <type> <object>
```

#### Description

**Auto mode** takes one object name and a single inspection flag. qgit reads the object and prints the requested detail.

**Raw mode** takes a type and an object name. qgit checks that the object type matches, then prints the raw payload bytes.

Only one of `-p`, `-t`, or `-s` may be used at a time. The command must be run inside a qgit repository.

Supported types: `blob`, `commit`, `tree`, `tag`.

#### Options

`-h`
`--help`

Show help message and exit.

`-p`

Pretty-print the object contents.

`-t`

Print the object type.

`-s`

Print the object size in bytes.

`<type>`

Object type for raw mode.

`<object>`

Object name, usually a SHA-1 hash.

---

### check-ignore

Debug gitignore and exclude files.

---

### checkout

Switch branches or restore working tree files.

---

### commit

Record changes to the repository.

---

### config

Get and set repository or global options.

Keys use the form `<section>.<name>`.

#### Synopsis

```
qgit config [<scope>] --list
qgit config [<scope>] --get <key>
qgit config [<scope>] --set <key> <value>
qgit config [<scope>] --unset <key>
```

`<scope>` is optional. Use `--global` or `--local` to target one file. With no scope, qgit uses both files where the action allows.

#### Description

Reads and writes qgit configuration. The global file lives in the user home directory. The local file belongs to the current repository.

With no scope:

* `--list` prints global entries first, then local entries
* `--get` reads local first, then falls back to global
* `--set` writes to the local repository config
* `--unset` removes the key from the local repository config

`--local` requires a qgit repository in the current directory or above. `--set` without scope also requires a repository.

A missing key makes `--get` exit with status 1. A missing key makes `--unset` exit with status 1 when the selected config file is present.

#### Options

`-h`
`--help`

Show help message and exit.

`--global`

Use the global config file only.

`--local`

Use the local repository config file only.

`-l`
`--list`

List all variables for the selected scope.

`-g`
`--get`

Print the value of `<key>`.

`-s`
`--set`

Set `<key>` to `<value>`.

`-u`
`--unset`

Remove `<key>`.

---

### hash-object

Compute object ID and optionally create a blob from a file.

---

### help

Display help information about Git.

#### Synopsis

```
qgit help
```

#### Description

Prints usage and a list of available qgit subcommands with brief descriptions. Running `qgit` with no subcommand shows the same output.

Extra arguments are ignored.

---

### init

Create an empty Git repository or reinitialize an existing one.

The default initial branch is `main`.

#### Synopsis

```
qgit init [-q | --quiet] [-b <branch-name> | --initial-branch=<branch-name>] [<directory>]
```

#### Description

Creates an empty repository with a `.qgit` directory and an initial branch with no commits. Running `qgit init` in an existing repository is safe and will not overwrite existing data.

#### Options

`-h`
`--help`

Show help message and exit.

`-q`
`--quiet`

Only print error and warning messages. All other output is suppressed.

`-b` `<branch-name>`
`--initial-branch=` `<branch-name>`

Use `<branch-name>` for the initial branch in the newly created repository. Defaults to `main`.

`<directory>`

Create the repository in `<directory>`. Defaults to the current directory.

---

### log

Show commit logs.

#### Synopsis

```
qgit log [--oneline] [--first-parent] [-n <num>] [<commit>]
```

#### Description

List commits reachable from `<commit>`, or from `HEAD` when omitted, by following parent links. The command must be run inside a qgit repository.

By default, qgit prints one entry per commit with author, date, and message. Merge commits with more than one parent include a `Merge:` line listing abbreviated parent hashes.

#### Options

`-h`
`--help`

Show help message and exit.

`--oneline`

Show each commit on a single line with an abbreviated hash and the first line of the commit message.

`--first-parent`

When walking history, follow only the first parent of a merge commit.

`-n` `<num>`

Limit the number of commits shown.

`<commit>`

Start listing from this commit. Defaults to `HEAD`.

---

### ls-files

Show information about files in the index and working tree.

---

### ls-tree

List the contents of a tree object.

#### Synopsis

```
qgit ls-tree [-r] [<tree-ish>]
```

#### Description

Lists the contents of a given tree object. When `<tree-ish>` names a commit, qgit lists the tree recorded in that commit. The command must be run inside a qgit repository.

Without `-r`, qgit lists only the entries at the top level of the tree. With `-r`, qgit recurses into sub-trees and prints blob paths with parent directory prefixes. The `-t` flag applies only with `-r` and makes qgit print tree entries for directories being recursed into.

Each line uses the default Git format: object mode, object type, object name, a tab, and the path name.

`<tree-ish>` may be a full SHA-1 hash, `HEAD`, or a branch name under `refs/heads`. When omitted, qgit uses `HEAD`. qgit does not accept path arguments or abbreviated object names.

#### Options

`-h`
`--help`

Show help message and exit.

`-r`

Recurse into sub-trees.

`-t`

Show tree entries even when going to recurse them. Has no effect unless `-r` is also given.

`<tree-ish>`

Tree or commit to list. Defaults to `HEAD`.

---

### rev-parse

Pick out and massage parameters.

---

### rm

Remove files from the working tree and from the index.

---

### show-ref

List references in a local repository.

#### Synopsis

```
qgit show-ref [--head] [--branches] [--tags]
```

#### Description

Displays references available in a local repository along with the associated commit IDs. The command must be run inside a qgit repository.

By default, qgit lists local branch and tag references under `refs/heads` and `refs/tags`. Output is one line per reference in the form `<oid> <ref>`.

qgit reads loose refs only. Remote references and `packed-refs` are not supported.

#### Options

`-h`
`--help`

Show help message and exit.

`--head`

Show the `HEAD` reference, even if it would normally be filtered out.

`--branches`
`--tags`

Limit to local branches and local tags, respectively. These options are not mutually exclusive. When both are given, references stored in `refs/heads` and `refs/tags` are displayed.

---

### status

Show the working tree status.

---

### tag

Create, list, or delete tags.

qgit supports lightweight tags only. Annotated tags, signing, and verification are not supported.

#### Synopsis

```
qgit tag [options] <tagname> [<commit>]
qgit tag [options] -d <tagname>
```

#### Description

Create, list, or delete tag references in `refs/tags/`. The command must be run inside a qgit repository.

With no arguments, qgit lists all tags. With `<tagname>`, qgit creates a lightweight tag pointing at `<commit>`, or at `HEAD` when `<commit>` is omitted. With `-d`, qgit deletes the named tag.

Creating a tag with an existing name overwrites the previous value.

#### Options

`-h`
`--help`

Show help message and exit.

`-l`
`--list`

List tags. Running `qgit tag` with no arguments also lists tags.

`-d`
`--delete`

Delete the tag with the given name.

`<tagname>`

The name of the tag to create, list, or delete.

`<commit>`

The object that the new tag will refer to, usually a commit. Defaults to `HEAD`.

---

### version

Show the version.

#### Synopsis

```
qgit version
```

#### Description

Prints the qgit version as `qgit version <major>.<minor>.<patch>`.

Extra arguments are ignored.

## Contribution

Issues and pull requests are welcome. Please keep changes focused and run `make test` before submitting. This project is licensed under GPLv3. See [LICENSE](LICENSE) for details.
