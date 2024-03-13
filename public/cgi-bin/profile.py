#!/usr/local/bin/python3
import warnings
warnings.filterwarnings("ignore")
import cgi
import json
import os
import random
import string
import sys
import hashlib
import base64
import time

sessions_path = "sessions/"
def read_and_decode_from_session(filename):
    # Read the encoded data from the file
    with open(filename, 'r') as file:
        encoded_data = file.read()

    # Decode the data using base64
    decoded_data = base64.b64decode(encoded_data).decode('utf-8')

    return decoded_data
def render_hello_page(existing_session_id):
    sid_path = sessions_path + existing_session_id
    username = read_and_decode_from_session(sid_path)

    if username:
        print("HTTP/1.1 200 OK")
        print("Content-Type: text/html")
        html_content = "<html><head><title>Hello, {}!</title></head><body><h2>Hello, {}!</h2></body></html>".format(username, username)
        print("Content-Length: ", len(html_content))
        print("\n" + html_content)
    else:
        print("HTTP/1.1 302 Found")
        print("Location: login.py")

def handle_profil():
    session_cookie = os.environ.get('SID', '')
    if session_cookie:
        render_hello_page(session_cookie)
    else:
        print("HTTP/1.1 302 Found")
        print("Location: login.py")
        print("\n")
if __name__ == '__main__':
    handle_profil()