import socket

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
    
    while True:
        # Wait for a client to connect
        client_socket, client_address = server_socket.accept()
        print(f'Connection established with {client_address}')
        
        # Prepare a simple HTTP response
#         http_response = b"""\
# HTTP/1.1 200 OK
# Content-Type: text/html
# Content-Length: 13

# Hello, world!
# Hey sweet buddy

# """
        http_response = b"""\
HTTP/1.1 200 OK\r\n
Date: Mon, 30 Sep 2024 12:00:00 GMT\r\n
Content-Type: text/html; charset=UTF-8\r\n
Cache-Control: max-age=
Content-Length: 138\r\n
Connection: close\r\n\r\n
<html>\r\n
<head><title>Example Page</title></head>\r\n
<body>\r\n
<h1>Welcome to the Example Page</h1>\r\n
<p>This is a simple HTTP response with correct formatting.</p>\r\n
</body>\r\n
</html>\r\n
"""
        # Send the response to the client
        client_socket.sendall(http_response)
        # Close the connection
        client_socket.close()

if __name__ == '__main__':
    start_server()