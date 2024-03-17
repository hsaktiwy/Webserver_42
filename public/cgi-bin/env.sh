#!/bin/bash

# Function to calculate the length of the response
calculate_content_length() {
    content="$1"
    echo -n "$content" | wc -c
}

# Content of the response
response_content=$(env)

# Calculate content length
content_length=$(calculate_content_length "$response_content")
# Print headers
echo "HTTP/1.1 200 OK"
echo "Content-type: text/plain"
echo "Content-Length: $content_length"
echo ""

# Print response content
echo "$response_content"
