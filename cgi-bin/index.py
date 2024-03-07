#!/usr/local/bin/python3
# import os
# import cgi
# form = cgi.FieldStorage()
# user_input = form.getvalue("user_input", "")
# print ("Content-type: text/html\r\n\r\n");
# print("<html>")
# print("<body>")
# print ("<font size=+1>Environment</font><br>");
# # print("Content-type: text/html\n")  # CGI header

# # print("<head>")
# # print("<title>Simple CGI Script</title>")
# # print("</head>")

# # # Process form data
# # # form = cgi.FieldStorage()
# # # user_input = form.getvalue("user_input", "")

# # # Display the input
# # print("<h1>Simple CGI Script</h1>")
# # # print("<p>You entered: {}</p>".format(user_input))

# # # A simple form to get user input
# # print("<form method='post' action=''>")
# # print("<label for='user_input'>Enter something:</label>")
# # print("<input type='text' name='user_input'>")
# # print("<input type='submit' value='Submit'>")
# # print("</form>")

# print("</body>")
# print("</html>")

import cgi
import os

print("HTTP/1.1 200 OK");
print("Content-type: text/html\n")  # CGI header

print("<html>")
print("<head>")
print("<title>CGI Script with FieldStorage</title>")
print("</head>")
print("<body>")
for param in os.environ.keys():
   print ("<b>%20s</b>: %s<br>" % (param, os.environ[param]))

# Create a FieldStorage object to parse data from the request


print("</body>")
print("</html>")

