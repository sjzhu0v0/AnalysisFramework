#!/usr/bin/env python3
import os
import argparse

# Set up argument parser
parser = argparse.ArgumentParser(description="Generate hadd commands for merging ROOT files.")
parser.add_argument("--commands", type=str, default="SE_PR_thn", help="List of commands separated by spaces (default: 'SE_PR_thn').")
parser.add_argument("--input_file", type=str, default="/lustre/alice/users/szhu/work/Analysis/InfoRun/Grouping/group1", help="Path to the input file (default: '/lustre/alice/users/szhu/work/Analysis/InfoRun/Grouping/group1').")
parser.add_argument("--output_dir", type=str, default="/lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/merge", help="Path to the output directory (default: '/lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/merge').")

# Parse arguments
args = parser.parse_args()

# Assign arguments to variables
commands = args.commands
input_file = args.input_file
output_dir = args.output_dir

# Parse command list
cmd_list = commands.split()

# Read input file
with open(input_file, 'r') as f:
    lines = f.readlines()

# Process each line
for line_idx, line in enumerate(lines, start=1):
    # Split fields (assuming comma + space separation)
    fields = [f.strip() for f in line.split(', ')]

    # Iterate over bootstrap samples (1-20)
    for j in range(1, 21):
        # Iterate over each command
        for cmd in cmd_list:
            # Build output file path
            output_file = f"{output_dir}/{cmd}/group{line_idx}_bs_{j}.root"

            # Build input file list
            input_files = " ".join(
                f"/lustre/alice/users/szhu/job/JpsiFlowPair/analysis_22highIR_pass4/{field}/{cmd}_bs_{j}.root"
                for field in fields
            )

            # Print hadd command
            print(f"hadd -f {output_file} {input_files}")