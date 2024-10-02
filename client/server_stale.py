import socket

# Server 1 run on port 9053
def start_server(host='10.4.2.20', port=9053):
    # Create a socket object
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Set the SO_REUSEADDR option to allow the reuse of an address in TIME_WAIT state
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # Bind the socket to the host and port
    server_socket.bind((host, port))
    # Listen for incoming connections (max 5 queued connections)
    server_socket.listen(5)

    print(f'Server started on {host}:{port}')

    # Counter to track the number of requests handled
    request_count = 0

    while True:
        # Wait for a client to connect
        client_socket, client_address = server_socket.accept()
        print(f'Connection established with {client_address}')

        # Receive data from the client (up to 1024 bytes)
        request_data = client_socket.recv(1024).decode('utf-8')

        # Check if the request is a GET request
        if request_data.startswith('GET'):
            print("Received GET request from client:")
            print(request_data)

            # Increment the request count
            request_count += 1

            # Serve different content based on the request count
            if request_count <= 2:
                # Original content for the first two connections
                http_response = b"""\
HTTP/1.1 200 OK\r
Date: Mon, 30 Sep 2024 12:00:00 GMT\r
Content-Type: text/html; charset=UTF-8\r
Cache-Control: max-age=1\r
Content-Length: 138\r
Connection: close\r\n\r
<html>\r\n
<head><title>Example Page</title></head>\r\n
<body>\r\n
<h1>Welcome to SERVER 1 Example Page</h1>\r\n
<p>This is a simple HTTP response with correct formatting.</p>\r\n
</body>\r\n
</html>\r\n
"""
            else:
                # Different content for every connection after the first two
                http_response = b"""\
HTTP/1.1 200 OK\r
Date: Mon, 30 Sep 2024 12:00:00 GMT\r
Content-Type: text/html; charset=UTF-8\r
Content-Length: 156\r
Connection: close\r\n\r
<html>\r\n
<head><title>Different Content Page</title></head>\r\n
<body>\r\n
<h1>Welcome to the Alternate Server Page</h1>\r\n
<p>This is a different response sent after the first two connections.</p>\r\n
</body>\r\n
</html>\r\n
"""
            # Send the HTTP response based on the request count
            client_socket.sendall(http_response)
        else:
            print("Received a non-GET request or no request from client.")

        # Close the client socket
        client_socket.close()

if __name__ == '__main__':
    start_server()

#Cache-Control: max-age=25\r

