#!/usr/local/bin/python3
import os
# Set the desired file path manually
current_directory = os.getcwd()
file_path = os.path.join(current_directory, 'storage/Makefile')

# HTML Response
html_response = "<html><head><title>File Open Status</title></head><body>"

if file_path:
    try:
        # Attempt to open the file
        with open(file_path, 'r') as file:
            content = file.read()
            html_response += f"<h1>File Opened Successfully</h1><p>Content:</p><pre>{content}</pre>"
    except FileNotFoundError:
        html_response += f"<h1>Error: File Not Found</h1><pre>{current_directory}</pre>"
else:
    html_response += "<h1>Error: No File Path Specified</h1>"

html_response += "</body></html>"

# Calculate Content-length dynamically
content_length = len(html_response)

# Print Content-type and Content-length headers
print("HTTP/1.1 200 OK")
print("Content-type: text/html")
print(f"Content-length: {content_length}")
print()  # Add an extra line break before the HTML content

# Print the HTML response
print(html_response)
