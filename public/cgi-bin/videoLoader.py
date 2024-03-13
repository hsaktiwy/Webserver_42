#!/usr/local/bin/python3
import os
print("HTTP/1.1 200 OK");

print("Content-type: text/html")  # CGI header
print("Content-Length: 392\n")  # CGI header

# Replace with the actual path to your video file
video_path = "../video/ss.mp4"

print("<html>")
print("<head>")
print("<title>Video Page</title>")
print("</head>")
print("<body>")

print("<h1>Video 1</h1>")
print("<video width='640' height='360' controls>")
print("<source src='{}' type='video/mp4'>".format(video_path))
print("Your browser does not support the video tag.")
print("</video>")
print("<h1>Video 2</h1>")
print("<video width='640' height='360' controls>")
print("<source src='{}' type='video/mp4'>".format(video_path))
print("Your browser does not support the video tag.")
print("</video>")

print("</body>")
print("</html>")
