#!/usr/local/bin/python3
import os
import datetime
import time
import cgitb
cgitb.enable()
print("HTTP/1.1 200 OK")
print("Content-Length: 156")
print("Content-Type: text/html\n")

current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

print("<html>")
print("<head>")
print("<title>Simple CGI Script</title>")
print("</head>")
print("<body>")
print("<h1>Simple CGI Script in Python</h1>")
print("<p>Current Time: {}</p>".format(current_time))
print("</body>")
print("</html>")
