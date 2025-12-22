#!/usr/bin/env python3
import argparse
import sys


def read_lines_from_file(filename):
    with open(filename, "r") as f:
        return [line.rstrip("\n") for line in f if line.strip()]


def read_lines_from_stdin():
    return [line.rstrip("\n") for line in sys.stdin if line.strip()]


def find_global_constant_mask(strings):
    """
    True  -> 所有字符串在该位置字符完全一致（非差异位）
    False -> 存在差异
    """
    max_len = max(len(s) for s in strings)
    mask = []

    for i in range(max_len):
        chars = set()
        for s in strings:
            if i < len(s):
                chars.add(s[i])
            else:
                chars.add(None)
        mask.append(len(chars) == 1)

    return mask


def extract_multi_differences(strings, debug_mask=False):
    mask = find_global_constant_mask(strings)

    if debug_mask:
        mask_str = "".join("1" if m else "0" for m in mask)
        print("GLOBAL MASK:", mask_str, file=sys.stderr)

    results = []

    for s in strings:
        parts = []
        current = []

        for i, same in enumerate(mask):
            c = s[i] if i < len(s) else ""

            if not same:
                current.append(c)
            else:
                if current:
                    parts.append("".join(current))
                    current = []

        if current:
            parts.append("".join(current))

        results.append("_".join(parts))

    return results


def main():
    parser = argparse.ArgumentParser(
        description="Extract global character differences from strings"
    )
    parser.add_argument(
        "-i", "--input",
        help="Input file (one string per line). If omitted, read from stdin."
    )
    parser.add_argument(
        "--debug-mask",
        action="store_true",
        help="Print global constant mask to stderr for debugging"
    )

    args = parser.parse_args()

    if args.input:
        lines = read_lines_from_file(args.input)
    else:
        lines = read_lines_from_stdin()

    if not lines:
        sys.exit(0)

    diffs = extract_multi_differences(lines, debug_mask=args.debug_mask)

    for d in diffs:
        print(d)


if __name__ == "__main__":
    main()
