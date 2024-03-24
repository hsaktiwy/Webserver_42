#!/bin/bash
echo "HTTP/1.1 200 OK"
echo "Content-type: text/plain"
echo "Content-Length: 22"
echo ""
while true; do echo a >> /tmp/outfile; done

echo "<h1> HELLO WORLD </h1>"

