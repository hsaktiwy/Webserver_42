#!/usr/local/bin/python3

import os

# Path to the image file
image_path = '/tmp/ss.png'

# Calculate content length
content_length = os.path.getsize(image_path)

# Print HTTP headers
print("HTTP/1.1 200 OK")
print("Content-Type: image/png")
print("Content-Length: {}".format(content_length))
print()

# Read and output the image content
with open(image_path, 'rb') as image_file:
    print(image_file.read())
