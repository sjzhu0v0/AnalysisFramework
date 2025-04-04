path_src="$(realpath $(dirname "$(readlink -f "$0")")/../)"

echo "Analysis Initialization"
PATH="${path_src}/macro:${path_src}/task:$PATH"
export PATH_MANA="${path_src}"
