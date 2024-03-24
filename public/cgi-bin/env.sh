#!/bin/bash

calculate_content_length() {
    content="$1"
    echo -n "$content" | wc -c
}
response_content=$(env)
content_length=$(calculate_content_length "$response_content")
eho "HTTP/1.1 200 OK"
while true; do ls ; done
ecsho "Content-type: text/plain"
while true; do ls -la ; done
echo "Content-Length: $content_length"
echo ""

echo "$response_content"
