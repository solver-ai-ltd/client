#!/bin/bash

# Get the directory of the script
DIR="$(dirname "$0")"

# Navigate to the Client/js directory relative to the script directory
cd "$DIR/js"

# Run npm install
npm install
