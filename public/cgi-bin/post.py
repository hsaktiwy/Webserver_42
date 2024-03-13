#!/usr/local/bin/python3
import cgi

# Create an instance of FieldStorage
form = cgi.FieldStorage()
if "name" not in form or "addr" not in form:
    print("<H1>Error</H1>")
    print("Please fill in the name and addr fields.")
print("<p>name:", form["name"].value)
print("<p>addr:", form["addr"].value)
