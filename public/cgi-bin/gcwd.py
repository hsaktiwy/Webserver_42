#!/usr/local/bin/python3
import os

# Set the content type and get the current working directory
content_type = "text/plain"
current_directory = os.getcwd() 

# Calculate the content length
content_length = len(current_directory)

# Print the HTTP headers
print("HTTP/1.1 200 OK");
print("Content-Type: {}".format(content_type))
print("Content-Length: {}".format(content_length))
print()  # Empty line to indicate the end of headers

# Print the current working directory
print(current_directory)
