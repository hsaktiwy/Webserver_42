#!/bin/bash

WEBSITE_URL=$1  # Replace with your website URL
CONCURRENT_USERS=$2  # Adjust the number of concurrent users as needed
TEST_DURATION=$3 # Adjust the test duration as needed (e.g., "1M" for 1 minute)

# Run Siege command with specified parameters
siege -c$CONCURRENT_USERS -t$TEST_DURATION $WEBSITE_URL