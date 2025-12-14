#!/usr/bin/env bash
set -e

DEBUG=0

usage() {
  echo "用法:"
  echo "  $0 \"since\" \"until\""
  echo "  $0 -d \"since\" \"until\"   # debug 模式（不执行 rebase）"
  echo ""
  echo "示例:"
  echo "  $0 \"2024-06-01\" \"2024-06-05\""
  echo "  $0 -d \"2024-06-01\" \"2024-06-05\""
}

# 参数解析
if [ "$1" == "-d" ]; then
  DEBUG=1
  shift
fi

if [ $# -ne 2 ]; then
  usage
  exit 1
fi

SINCE="$1"
UNTIL="$2"

echo "▶ 时间范围: $SINCE ~ $UNTIL"
[ $DEBUG -eq 1 ] && echo "▶ DEBUG 模式开启（不会修改历史）"

# 找到时间段内的 commit（从旧到新）
COMMITS=$(git log --since="$SINCE" --until="$UNTIL" --reverse --format="%H")

if [ -z "$COMMITS" ]; then
  echo "❌ 该时间段内没有 commit"
  exit 0
fi

echo "▶ 命中的 commits:"
git log --oneline --since="$SINCE" --until="$UNTIL"

FIRST_COMMIT=$(echo "$COMMITS" | head -n 1)
BASE_COMMIT=$(git rev-parse "${FIRST_COMMIT}^")

echo "▶ rebase 起点: $BASE_COMMIT"

# 构造 sed 正则
SED_PATTERN=$(echo "$COMMITS" | tr '\n' '|' | sed 's/|$//')

if [ $DEBUG -eq 1 ]; then
  echo ""
  echo "▶ DEBUG: 将 drop 以下 commits："
  echo "$COMMITS"
  echo ""
  echo "▶ DEBUG: sed 匹配表达式："
  echo "$SED_PATTERN"
  echo ""
  echo "▶ DEBUG: rebase todo 预览："
  git rebase -i --rebase-merges --autosquash "$BASE_COMMIT" --dry-run 2>/dev/null || true
  echo ""
  echo "✅ DEBUG 模式结束（未执行 rebase）"
  exit 0
fi

# 真正执行 rebase
GIT_SEQUENCE_EDITOR="sed -i.bak '/$SED_PATTERN/s/^pick /drop /'" \
git rebase -i "$BASE_COMMIT"

git reflog expire --expire=now --all
git gc --prune=now --aggressive


echo "✅ 删除完成"

