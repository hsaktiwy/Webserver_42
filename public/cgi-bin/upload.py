#!/usr/local/bin/python3
import warnings
warnings.filterwarnings("ignore")
import cgi
import os

def calculate_content_length(data):
    return len(data.encode('utf-8'))

def redirect_upload():
    print("HTTP/1.1 302 Found")
    print("Location: ../fileUpload.html")
    print("\n")

request_method =os.environ.get("REQUEST_METHOD", '')
if request_method:
    if request_method == 'GET':
        redirect_upload()
    else:
        path = os.environ.get("UPLOADS", '')
        form = cgi.FieldStorage()

        fileitem = form['file']


        if fileitem.filename:
            # Get the file data
            file_content = fileitem.file.read()
            
            # Process the file content as needed
            # For example, you can save it to a file
            with open(path + os.path.basename(fileitem.filename), 'wb') as f:
                f.write(file_content)

            dynamic_content = """
            <html>
            <head>
                <title>File Upload Result</title>
            </head>
            <body>
                <h1>File Upload Successful</h1>
                <p>Uploaded file name: {}</p>
            </body>
            </html>
            """.format(format(fileitem.filename))  
        else:

            dynamic_content = """
            <html>
            <head>
            <title>File Upload Result</title>
            </head>
            <body>
            <h1>No file uploaded</h1>
            </body>
            </html>
            """.format(data)
        print("HTTP/1.1 200 OK")
        print("Content-type: text/html")
        print("Content-length: {}\n".format(calculate_content_length(dynamic_content)))
        print(dynamic_content)

