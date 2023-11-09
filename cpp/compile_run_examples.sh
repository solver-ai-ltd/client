#!/bin/bash

dir="$(dirname "$0")"

make -f Makefile

# Find executables starting with "example_" in the script's directory
executables=($(find "$dir" -name "example_*" -executable))

# Loop over each executable
for exec in "${executables[@]}"
do
  ./$exec
  # Get the exit status of the last command
  status=$?
  if [ $status -eq 0 ]
  then
    echo "$exec exited with status 0"
  else
    echo "$exec exited with status $status"
  fi
done
