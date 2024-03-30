# Webserv : An HTTP Web Server with C++

This project aims to develop a high-performance web server in C++ capable of handling asynchronous non-blocking I/O operations for improved scalability. The server supports both static and dynamic content responses and implements a CGI interface for dynamic content generation. Additionally, the server manages cookies and sessions to facilitate stateful interactions with clients.

## Features

- **Asynchronous Monitoring:** Utilizes asynchronous programming techniques to handle non-blocking I/O operations efficiently, allowing the server to handle a large number of concurrent connections without blocking.

- **Static Content Serving:** Capable of serving static content such as HTML, CSS, JavaScript, images, and other files directly to clients.

- **Dynamic Content Generation:** Implements a Common Gateway Interface (CGI) to generate dynamic content in response to client requests. Supports executing external scripts or programs to generate content on-the-fly.

- **Cookie and Session Management:** Provides mechanisms for managing cookies and sessions, enabling stateful interactions with clients across multiple requests.



