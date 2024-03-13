#!/usr/local/bin/python3
import warnings
warnings.filterwarnings("ignore")
# import cgi
# import json

# # Path to the JSON file
# json_file_path = 'user_data.json'

# # Load existing user data
# def load_user_data():
#     try:
#         with open(json_file_path, 'r') as file:
#             data = json.load(file)
#     except FileNotFoundError:
#         data = []
#     return data

# # Save user data to the JSON file
# def save_user_data(user_data):
#     with open(json_file_path, 'w') as file:
#         json.dump(user_data, file, indent=2)

# # Process form submission
# form = cgi.FieldStorage()

# if 'username' in form and 'password' in form:
#     username = form.getvalue('username')
#     password = form.getvalue('password')
#     email = form.getvalue('email')

#     # Load existing user data
#     user_data = load_user_data()

#     # Check if the username is already registered
#     if any(user['username'] == username for user in user_data):
#         print("Content-type: text/html\n")
#         print("<html><body><h2>Username already exists. Choose a different username.</h2></body></html>")
#     else:
#         # Add new user data
#         new_user = {'username': username, 'email': email ,'password': password}
#         user_data.append(new_user)

#         # Save updated user data
#         save_user_data(user_data)

#         print("Content-type: text/html\n")
#         print("<html><body><h2>Registration successful. Welcome, {}!</h2></body></html>")
# else:
#     print("Content-type: text/html\n")
#     print("<html><body><h2>Invalid form submission.</h2></body></html>")
import cgi
import os
import json

# Path to the JSON file
root = os.environ.get('ROOT')
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
form = cgi.FieldStorage()

if 'username' in form and 'password' in form and 'email' in form:
    username = form.getvalue('username')
    password = form.getvalue('password')
    email = form.getvalue('email')

    # Load existing user data
    user_data = load_user_data()

    # Check if the username is already registered
    if any(user['username'] == username for user in user_data) or any(user['email'] == email for user in user_data):
        print("HTTP/1.1 200 OK")
        print("Content-type: text/html")
        html_content = "<html><body><h2>Username or Email already exist !</h2><p> <a href=\"../register.html\"> Try again! </a> </p></body></html>"
        print("Content-length:", len(html_content))
        print("\n" + html_content)
    else:
        # Add new user data
        new_user = {'username': username, 'password': password, 'email': email}
        user_data.append(new_user)

        # Save updated user data
        save_user_data(user_data)

        print("HTTP/1.1 200 OK");
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
