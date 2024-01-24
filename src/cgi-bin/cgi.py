#!/usr/bin/python3

#!/usr/bin/env python
import os

import sys
image_path = '/Users/aalami/Desktop/Webserver_42/cgi-bin/kitty.png'

# Get the size of the image file
image_size = os.path.getsize(image_path)

# Send the status line
sys.stdout.buffer.write(b"HTTP/1.1 3 OK\r\n")
# Send the Content-type header indicating that it's an image
sys.stdout.buffer.write(b"Content-type: image/png\r\n")
sys.stdout.buffer.write(b"Location: https://profile.intra.42.fr/\r\n")
sys.stdout.buffer.write(b"Transfer-Encoding: chunked\r\n\r\n")

# Open the image file and read its content in chunks
chunk_size = 1024  # You can adjust the chunk size based on your needs

with open(image_path, 'rb') as f:
    while True:
        chunk = f.read(chunk_size)
        if not chunk:
            break
        sys.stdout.buffer.write(f"{len(chunk):X}\r\n".encode('latin-1'))
        sys.stdout.buffer.write(chunk)
        sys.stdout.buffer.write(b"\r\n")

# Signal the end of the chunks
sys.stdout.buffer.write(b"0\r\n\r\n")
sys.stdout.flush()


# import os
# # # import cgi

# # print("Content-type: image/apng\r\n\r\n")

# # print("<html>")
# # print("<head>")


# # print("<html>")
# # print("<body>")
# # print("<h2>Hello cgi</h2>")


# # print("</body>")
# # print("</html>")

# # import cgi
# import cgitb
# cgitb.enable()

# # Send the Content-type header indicating that it's an image
# print("HTTP/1.1 200 OK")
# image_path = '/Users/aalami/Desktop/Webserver_42/cgi-bin/ab.avif'
# print("Content-type: image/avif")
# image_size = os.path.getsize(image_path) 
# print(f"Content-Length: {image_size}")

# # Open the image file and read its content
# with open(image_path, 'rb') as f:
#     image_data = f.read()

# # Output the image data
# print(image_data)


