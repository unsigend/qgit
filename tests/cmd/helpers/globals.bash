# Global variables

QGIT_BIN="$QGIT"

QGIT_META_DIR=".qgit"

QGIT_REPO_DIRS=(
    "objects"
    "refs/heads"
    "refs/tags"
)

QGIT_REPO_FILES=(
    "HEAD"
    "config"
    "description"
)

QGIT_DEFAULT_BRANCH="main"

QGIT_CONFIG_SECTION="[core]"

QGIT_CONFIG_ENTRIES=(
    "repositoryformatversion=0"
    "filemode=false"
    "bare=false"
)

QGIT_DESCRIPTION_LINE="Unnamed repository"
