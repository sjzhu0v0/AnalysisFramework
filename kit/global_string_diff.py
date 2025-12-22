#!/usr/bin/env python3
import argparse
import sys


def read_lines_from_file(filename):
    with open(filename, "r") as f:
        return [line.rstrip("\n") for line in f if line.strip()]


def read_lines_from_stdin():
    return [line.rstrip("\n") for line in sys.stdin if line.strip()]


def longest_common_prefix(strings):
    if not strings:
        return ""
    prefix = strings[0]
    for s in strings[1:]:
        i = 0
        while i < len(prefix) and i < len(s) and prefix[i] == s[i]:
            i += 1
        prefix = prefix[:i]
    return prefix


def longest_common_suffix(strings):
    rev = [s[::-1] for s in strings]
    return longest_common_prefix(rev)[::-1]


def find_global_constant_mask(strings):
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
    # ===== Stage 1: 稳定剥离前缀 & 后缀 =====
    prefix = longest_common_prefix(strings)
    suffix = longest_common_suffix(strings)

    cores = [
        s[len(prefix): len(s) - len(suffix) if suffix else len(s)]
        for s in strings
    ]

    # ===== Stage 2: 仅在 core 上做全局 diff =====
    mask = find_global_constant_mask(cores)

    if debug_mask:
        print("CORE PREFIX STRIPPED:", prefix, file=sys.stderr)
        print("CORE SUFFIX STRIPPED:", suffix, file=sys.stderr)
        print(
            "CORE MASK:",
            "".join("1" if m else "0" for m in mask),
            file=sys.stderr
        )

    results = []

    for s in cores:
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
        description="Extract global multi-segment differences from strings"
    )
    parser.add_argument(
        "-i", "--input",
        help="Input file (one string per line). If omitted, read from stdin."
    )
    parser.add_argument(
        "--debug-mask",
        action="store_true",
        help="Print global mask info to stderr"
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
