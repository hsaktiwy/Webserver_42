#!/bin/bash

calculate_content_length() {
    content="$1"
    echo -n "$content" | wc -c
}
response_content=$(env)
content_length=$(calculate_content_length "$response_content")
echo "HTTP/1.1 200 OK"
echo "Content-type: text/plain"
sleep 10;
echo "Content-Length: $content_length"
echo ""

echo "$response_content"
