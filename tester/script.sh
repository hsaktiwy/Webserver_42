#!/bin/bash

# Define variables
file_path="out"
fail=0
path_webserver="../webserv"
test_cases=(
  "http1.conf"
)

echo "ok" > "True"

# Run tests and check results
for config_file in "${test_cases[@]}"; do
  "$path_webserver" "$config_file" > "out_${config_file//./_}"
  diff "out_${config_file//./_}" "True" > "$file_path"

  if [[ ! -s "$file_path" ]]; then
    echo "Test case with configuration '$config_file': PASSED"
  else
    echo "Test case with configuration '$config_file': FAILED"
    fail=$((fail + 1))
    cat "out_${config_file//./_}"
  fi
done

echo "Number of fails: $fail"