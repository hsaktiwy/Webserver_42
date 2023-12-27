#!/bin/bash

# Define the data
data=(
    "px,3880,py,5020,px2,3226,py2,5067"
    "px,3879,py,5019,px2,3227,py2,5066"
    "px,3869,py,5009,px2,3237,py2,5056"
    "px,3859,py,4999,px2,3247,py2,5046"
    "px,3849,py,4989,px2,3257,py2,5036"
    "px,3839,py,4979,px2,3267,py2,5036"
    "px,3829,py,4969,px2,3277,py2,5036"
    "px,3819,py,4959,px2,3287,py2,5036"
    "px,3809,py,4949,px2,3297,py2,5036"
)

# Path to the executable
executable_path="./a.out"

# Iterate through the data and call the executable
for entry in "${data[@]}"; do
    # Split the line into individual arguments
    arguments=($(echo "$entry" | tr ',' ' '))
    # Run the command
    "$executable_path" "${arguments[@]}"
done