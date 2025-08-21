#!/bin/bash

PREV_JOBID=""

while IFS= read -r CMD || [[ -n "$CMD" ]]; do
  [[ -z "$CMD" ]] && continue
  if [[ -n "$PREV_JOBID" ]]; then
    CMD="$CMD --dependency=afterok:$PREV_JOBID"
  fi
  echo "Running: $CMD"
  OUTPUT=$($CMD)
  echo "$OUTPUT"
  JOBID=$(echo "$OUTPUT" | grep -oP 'Submitted batch job \K[0-9]+')
  if [[ -z "$JOBID" ]]; then
    echo "Fatal: no jobs found!!!!"
    exit 1
  fi
  PREV_JOBID=$JOBID
done < ${1}