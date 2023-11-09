#!/bin/bash

dir="$(dirname "$0")"

# Find Python files starting with "example_" in the script's directory
python_files=($(find "$dir" -name "example_*.py"))

# Loop over each Python file
for py_file in "${python_files[@]}"
do
  python $py_file
  # Get the exit status of the last command
  status=$?
  if [ $status -eq 0 ]
  then
    echo "$py_file exited with status 0"
  else
    echo "$py_file exited with status $status"
  fi
done
