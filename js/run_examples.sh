#!/bin/bash

# Get the directory of the script
dir="$(dirname "$0")"

# Find JavaScript files starting with "example_" in the script's directory
js_files=($(find "$dir" -name "example_*.js"))

# Loop over each JavaScript file
for js_file in "${js_files[@]}"
do
  node $js_file
  # Get the exit status of the last command
  status=$?
  if [ $status -eq 0 ]
  then
    echo "$js_file exited with status 0"
  else
    echo "$js_file exited with status $status"
  fi
done
