#!/usr/local/bin/python3
import warnings
warnings.filterwarnings("ignore")
import cgi
import os
import json
import random
import string
import sys
import hashlib
import base64
import time
root = os.environ.get('ROOT')
users_data_path = root + "/user_data.json"
sessions_path = "sessions/"
# Path to the JSON file
json_file_path = root + "/user_data.json"

# Load existing user data
def load_user_data():
    try:
        with open(json_file_path, 'r') as file:
            data = json.load(file)
    except FileNotFoundError:
        data = []
    return data

# Save user data to the JSON file
def save_user_data(user_data):
    with open(json_file_path, 'w') as file:
        json.dump(user_data, file, indent=2)

# Process form submission
def read_and_decode_from_session(filename):
    # Read the encoded data from the file
    with open(filename, 'r') as file:
        encoded_data = file.read()

    # Decode the data using base64
    decoded_data = base64.b64decode(encoded_data).decode('utf-8')

def redirect_profile(existing_session_id):
    # Load user data from JSON file
    sid_path = sessions_path + existing_session_id
    username = read_and_decode_from_session(sid_path)

    if username:
        print("HTTP/1.1 302 Found")
        print("Location: profile.py")
        print("\n")
    else:
        print("HTTP/1.1 302 Found")
        print("Location: login.py")
        print("\n")

def redirect_register():

    print("HTTP/1.1 302 Found")
    print("Location: ../register.html")
    print("\n")

def handle_register():

    request_method = os.environ.get('REQUEST_METHOD','')
    if request_method == "GET":
        session_cookie = os.environ.get('SID', '')
        if session_cookie:
            redirect_profile(session_cookie)
        else:
            redirect_register()
    else:
        form = cgi.FieldStorage()

        if 'username' in form and 'password' in form and 'email' in form:
            username = form.getvalue('username')
            password = form.getvalue('password')
            email = form.getvalue('email')
            user_data = load_user_data()
            if any(user['username'] == username for user in user_data) or any(user['email'] == email for user in user_data):
                print("HTTP/1.1 200 OK")
                print("Content-type: text/html")
                html_content = "<html><body><h2>Username or Email already exist !</h2><p> <a href=\"../register.html\"> Try again! </a> </p></body></html>"
                print("Content-length:", len(html_content))
                print("\n" + html_content)
            else:
                new_user = {'username': username, 'password': password, 'email': email}
                user_data.append(new_user)
                save_user_data(user_data)

                print("HTTP/1.1 201 Created");
                print("Content-type: text/html")
                html_content = "<html><body><h2>Registration successful you can login now!</h2><p> <a href=\"login.py\"> Login </a> </p></body></html>"
                print("Content-length:", len(html_content))
                print("\n" + html_content)
        else:
            print("HTTP/1.1 400 Bad Request")
            print("Content-type: text/html")
            html_content = "<html><body><h2 style=\"text-align: center;\">400 Bad Request!</h2></body></html>"
            print("Content-length: ", len(html_content))
            print("\n" + html_content)

if __name__ == '__main__':
    handle_register()