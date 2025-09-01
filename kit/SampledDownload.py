#!/usr/bin/env python3
import sys
import re
import subprocess

size_GB = 1024 * 1024 * 1024


def get_file_size(file):
    result = subprocess.run(["alien_stat", file], capture_output=True, text=True)
    match = re.search(r"Size:\s+(\d+)", result.stdout)
    if match:
        return int(match.group(1))
    else:
        return 0


def process_run(files, output, size_threshold):
    size_total = 0
    for i in files:
        size_total += get_file_size(i)
        print(i, file=output)

        if size_total >= size_threshold * size_GB:
            break


def process(input_files, output, runs_needed, size_threshold):
    for run in runs_needed:
        files4run = [f for f in input_files if re.search(run, f)]
        process_run(files4run, output, size_threshold)


def main():
    input_files = sys.argv[1]
    output_files = sys.argv[2]
    runs_needed = sys.argv[3]
    size_threshold = int(sys.argv[4])
    files = []
    with open(input_files, "r") as f:
        files = f.read().splitlines()
    runs = []
    with open(runs_needed, "r") as f:
        runs = f.read().splitlines()

    output = open(output_files, "w")
    process(files, output, runs, size_threshold)


if __name__ == "__main__":
    main()
