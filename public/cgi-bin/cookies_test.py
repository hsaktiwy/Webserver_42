#!/usr/local/bin/python3

from http.cookies import SimpleCookie
import os
import cgi


def redirect_cookie_page():
    print("HTTP/1.1 302 Found")
    print("Location: ../cookie.html")
    print("\n")

http_cookies = os.environ.get('HTTP_COOKIE', '')
request_method = os.environ.get('REQUEST_METHOD', '')
cookie = SimpleCookie()

if request_method == "GET":
    redirect_cookie_page()
else:
    form = cgi.FieldStorage()
    cookie_key = form.getvalue('cookie_key')
    cookie_value = form.getvalue('cookie_value')
    cookie_get = form.getvalue('cookie_key_getter')
    if cookie_key and cookie_value:
        cookie[cookie_key] = cookie_value
        cookie[cookie_key]['path'] = '/'
        cookie[cookie_key]['expires'] = 3600
        print("HTTP/1.1 200 OK")
        print("Set-Cookie: " + cookie.output(header=''))
        print("Content-Type: text/html")
        html_content = "<html><head><title>Cookies test</title></head><body><h2>Cookie {} setted </h2><a href=\"../cookie.html\">go back to check if setted</a></body></html>".format(cookie_key)
        print("Content-Length: ", len(html_content))
        print("\n" + html_content)
    elif cookie_get:
        if http_cookies:
            parse_cookies = SimpleCookie(http_cookies)
            value = None
            html_content = ""
            if cookie_get in parse_cookies:
               value = parse_cookies[cookie_get].value
               html_content = "<html><head><title>Cookies test</title></head><body><h2>The value of {} is {}</h2></body></html>".format(cookie_get, value)
            else:
                html_content = "<html><head><title>Cookies test</title></head><body><h2>Cookie {} not found</h2></body></html>".format(cookie_get)
            print("HTTP/1.1 200 OK")
            print("Content-Type: text/html")
            print("Content-Length: ", len(html_content))
            print("\n" + html_content)
        else:
            html_content = "<html><head><title>Cookies test</title></head><body><h2>Cookie {} not found</h2></body></html>".format(cookie_get)
            print("HTTP/1.1 200 OK")
            print("Content-Type: text/html")
            print("Content-Length: ", len(html_content))
            print("\n" + html_content)
    else:
        redirect_cookie_page()

        # for cookies in map(strip, split(environ['HTTP_COOKIE'], ';')):
        #     (key, value ) = split(cookies, '=');
        #     if key == cookie_get:
        #         value_get = value
        # html_content = "<html><head><title>Cookies test</title></head><body><h2>The value of {} is {}</h2></body></html>".format(cookie_get, value_get)
# # Parse existing cookies from the request
# def parse_cookies():
#     cookies = SimpleCookie()
#     if 'HTTP_COOKIE' in os.environ:
#         cookies.load(os.environ['HTTP_COOKIE'])
#     return cookies

# # Function to calculate the length of the HTML response
# def calculate_content_length(html_content):
#     return len(html_content.encode('utf-8'))

# # Get existing cookies
# existing_cookies = parse_cookies()

# # Set a new cookie value
# cookie = SimpleCookie()
# cookie['example_cookie'] = 'HelloCookie'
# cookie['example_cookie']['expires'] = 3600  # Cookie expires in 1 hour

# # Content of the HTML page
# html_content = """
# <html>
# <head>
#     <title>Cookie Test</title>
# </head>
# <body>
#     <h1>Cookie Test</h1>
# """

# # Display existing cookie value
# if 'example_cookie' in existing_cookies:
#     html_content += f"<p>Existing Cookie Value: {existing_cookies['example_cookie'].value}</p>"

# # Set the new cookie


# # HTML footer
# html_content += """
#     <p>Visit again to see the stored cookie value.</p>
# </body>
# </html>
# """

# # Calculate content length
# content_length = calculate_content_length(html_content)

# # Print the HTML header with Content-Length
# print("HTTP/1.1 201 OK")
# print(f"Content-type: text/html")
# print(f"Content-Length: {content_length}")
# print(cookie.output())
# print()

# # Print the content of the HTML page
# print(html_content)

