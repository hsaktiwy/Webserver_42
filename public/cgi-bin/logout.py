#!/usr/local/bin/python3

import cgi
import os
from http.cookies import SimpleCookie

sid = os.environ.get('SID','')
cookie = os.environ.get('HTTP_COOKIE','')
sessions_path = "sessions/"

sid_cookie = SimpleCookie()

if sid:
    current_session = sessions_path + sid
    os.remove(current_session)
    sid_cookie["SID"] = sid
    sid_cookie['SID']['expires'] = 'Thu, 01 Jan 1970 00:00:00 GMT'
    print("HTTP/1.1 302 Found")
    print("Set-Cookie: " + sid_cookie.output(header=''))
    print("Location: login.py")
    print("\n")
else:
    print("HTTP/1.1 302 Found")
    print("Location: login.py")
    print("\n")

