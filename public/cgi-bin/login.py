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

root = os.environ.get('ROOT')
users_data_path = root + "/user_data.json"
sessions_path = "sessions/"

# Function to generate a random session ID

def create_session_id(username):
    # Hash the username using SHA-256 (you can choose a different hash algorithm)
    data_to_hash = username + str(time.time())
    hashed_username = hashlib.sha256(data_to_hash.encode('utf-8')).hexdigest()
    return hashed_username

def generate_session_id():
    return ''.join(random.choices(string.ascii_letters + string.digits, k=32))

# Function to validate user credentials
def validate_user(username, password):
    # Load user data from a JSON file (replace with your actual file path)
    with open(users_data_path, 'r') as file:
        users = json.load(file)

    # Check if the provided username and password match any user in the data
    for user in users:
        if user['username'] == username and user['password'] == password:
            return True

    return False

# Function to render a page for the logged-in user
def read_and_decode_from_session(filename):
    # Read the encoded data from the file
    with open(filename, 'r') as file:
        encoded_data = file.read()

    # Decode the data using base64
    decoded_data = base64.b64decode(encoded_data).decode('utf-8')

    return decoded_data
def render_hello_page(existing_session_id):
    # Load user data from JSON file
    sid_path = sessions_path + existing_session_id
    username = read_and_decode_from_session(sid_path)

    if username:
        print("HTTP/1.1 200 OK")
        print("Content-Type: text/html")
        html_content = "<html><head><title>Hello, {}!</title></head><body><h2>Hello, {}!</h2></body></html>".format(username, username)
        print("Content-Length: ", len(html_content))
        print("\n" + html_content)
    else:
        print("HTTP/1.1 401 Unauthorized")
        print("Content-Type: text/html")
        html_content = "<html><head><title>401 Unauthorized </title></head><body><h2 style=\"text-align: center;\">401 Unauthorized</h2></body></html>"
        print("Content-Length: ", len(html_content))
        print("\n" + html_content)
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
        
def render_hello_page_first_time(new_session_id):

    print("HTTP/1.1 302 Found")
    print(f"Set-Cookie: SID={new_session_id}")
    print("Location: profile.py")
    print("\n")

def encode_and_write_to_session(data, filename):
    # Encode the data using base64
    encoded_data = base64.b64encode(data.encode('utf-8')).decode('utf-8')

    # Write the encoded data to the file
    with open(filename, 'w') as file:
        file.write(encoded_data)
def handle_login():

    request_method = os.environ.get('REQUEST_METHOD','')
    if request_method == "GET":
        session_cookie = os.environ.get('SID', '')
        if session_cookie:
            redirect_profile(session_cookie)
        else:
            print("HTTP/1.1 200 OK")
            print("Content-Type: text/html")
            html_content = """
                <!DOCTYPE html>
                <html lang="en">
                <head>
                    <meta charset="UTF-8">
                    <meta name="viewport" content="width=device-width, initial-scale=1.0">
                    <title>Login</title>
                    <link rel="stylesheet" type="text/css" href="../login.css">
                </head>
                <body>

                <form action="login.py" method="post" enctype="multipart/form-data">
                    <h1>Login Form</h1>
                    <label for="username">Username:</label>
                    <input type="text" id="username" name="username" required>
                    <label for="password">Password:</label>
                    <input type="password" id="password" name="password" required>
                    <br>
                    <input type="submit" value="Submit">

                    <!-- Styled button to link to the registration page -->
                    <a href="register2.py">Don't have an Account ? Register Now :)</a>
                </form>
                </body>
                </html>
                """
            print("Content-Length: ", len(html_content))
            print("\n" + html_content)
    else:
        form = cgi.FieldStorage()

        # Check if the request comes with a session cookie
        session_cookie = os.environ.get('SID', '')
        if session_cookie:
            redirect_profile(session_cookie)
        else:
            # Get input from the user
            username = form.getvalue('username')
            password = form.getvalue('password')

            # Validate user credentials
            if validate_user(username, password):
                # Generate a new session ID
                new_session_id = create_session_id(username)
                sid_path = sessions_path + new_session_id
                encode_and_write_to_session(username, sid_path)
                render_hello_page_first_time(new_session_id)
            else:
                print("HTTP/1.1 200 OK")
                print("Content-Type: text/html")
                html_content = "<html><head><title>Invalid Credentials</title></head><body><h2 style=\"text-align: center;\">Invalid Credentials</h2><p><a href=\"login.py\">Back to Login</a></p></body></html>"
                print("Content-Length: ", len(html_content))
                print("\n" + html_content)

# Main entry point
if __name__ == '__main__':
    handle_login()
