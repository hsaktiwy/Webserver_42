#!/bin/bash


content_length=$(ls -l public/cgi-bin/aa.jpeg| awk '{print $5}')



echo "HTTP/1.1 200 OK"
echo "Content-Type: image/jpeg"
echo "Content-Length: $content_length"
echo ""

cat public/cgi-bin/aa.jpeg