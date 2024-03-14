#!/Users/aalami/Desktop/WebServer2/public/cgi-bin/myenv/bin/python3
import warnings
warnings.filterwarnings("ignore")
import os
import cgi
import firebase_admin
from firebase_admin import credentials, db, firestore

cred = credentials.Certificate(r"/Users/aalami/Desktop/WebServer2/public/cgi-bin/dbtesting.json")
firebase_admin.initialize_app(cred)
form = cgi.FieldStorage()
username = form.getvalue('username')
password = form.getvalue('password')
email = form.getvalue('email')
db = firestore.client()
ref = db.collection('users')
# Get a reference to the Realtime Database

# Write data to the database
if username and password:
    user_data = {'username': username, 'password': password, 'email': email}
    ref.add(user_data)
    print("HTTP/1.1 200 OK");
    print("Content-length: 145")
    print("Content-type: text/html\n")
    print("<html>")
    print("<head>")
    print("<title>Registred Successfully</title>")
    print("</head>")
    print("<body>")
    print("<h1>Your account has been registred , You can login now !</h1>")
    print("</body>")
    print("</html>")
else:
    print("HTTP/1.1 200 OK")
    print("Content-length: 124")
    print("Content-type: text/html\n")
    print("<html>")
    print("<head>")
    print("<title>No data has been submitted </title>")
    print("</head>")
    print("<body>")
    print("<h1>No data has been submitted</h1>")
    print("</body>")
    print("</html>")

# print("Location: form.py");
# print("Content-Length: 175")  # CGI header
# print("Content-Type: text/html\n")  # CGI header

# print("<html>")
# print("<head>")
# print("<title>CGI Script Result</title>")
# print("<link rel='stylesheet' type='text/css' href='../style.css'>")
# print("</head>")
# print("<body>")

# # Retrieve form data from the QUERY_STRING environment variable


# # Display the input
# print("<p>Username: {}</p>".format(username))

# print("<p>Password: {}</p>".format(password))

# print("</body>")
# print("</html>")

# import cgi

# Initialize Firebase Admin SDK
# cred = credentials.Certificate(r"/Users/aalami/Desktop/WebServer2/public/cgi-bin/dbtesting.json")
# firebase_admin.initialize_app(cred)

# # Parse form data
# # form = cgi.FieldStorage()
# username = form.getvalue('username')
# password = form.getvalue('password')

# # Get a reference to the Realtime Database
# ref = db.reference('/users')

# # Write data to the database
# ref.child(username).set({
#     'password': password
# })

# print("Content-type: text/html\n")
# print("<html>")
# print("<head>")
# print("<title>Database Updated</title>")
# print("</head>")
# print("<body>")
# print("<h1>Database Updated</h1>")
# print("<p>Data written to the Firebase Realtime Database.</p>")
# print("</body>")
# print("</html>")







# import cgi
# import firebase_admin
# from firebase_admin import credentials, db, firestore


# # Filter out DeprecationWarning for 'cgi'

# # Initialize Firebase Admin SDK
# cred = credentials.Certificate(r"/Users/aalami/Desktop/WebServer2/public/cgi-bin/dbtesting.json")
# firebase_admin.initialize_app(cred)

# # Parse form data
# form = cgi.FieldStorage()
# username = form.getvalue('username')
# password = form.getvalue('password')
