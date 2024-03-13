#!/usr/local/bin/python3

from http.cookies import SimpleCookie
import os

# Parse existing cookies from the request
def parse_cookies():
    cookies = SimpleCookie()
    if 'HTTP_COOKIE' in os.environ:
        cookies.load(os.environ['HTTP_COOKIE'])
    return cookies

# Function to calculate the length of the HTML response
def calculate_content_length(html_content):
    return len(html_content.encode('utf-8'))

# Get existing cookies
existing_cookies = parse_cookies()

# Set a new cookie value
cookie = SimpleCookie()
cookie['example_cookie'] = 'HelloCookie'
cookie['example_cookie']['expires'] = 3600  # Cookie expires in 1 hour

# Content of the HTML page
html_content = """
<html>
<head>
    <title>Cookie Test</title>
</head>
<body>
    <h1>Cookie Test</h1>
"""

# Display existing cookie value
if 'example_cookie' in existing_cookies:
    html_content += f"<p>Existing Cookie Value: {existing_cookies['example_cookie'].value}</p>"

# Set the new cookie


# HTML footer
html_content += """
    <p>Visit again to see the stored cookie value.</p>
</body>
</html>
"""

# Calculate content length
content_length = calculate_content_length(html_content)

# Print the HTML header with Content-Length
print("HTTP/1.1 201 OK")
print(f"Content-type: text/html")
print(f"Content-Length: {content_length}")
print(cookie.output())
print()

# Print the content of the HTML page
print(html_content)

