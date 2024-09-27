#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUF_SIZE 8192

const char *html_response =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n\r\n"
    "<html><head><title>Simple C Web Server</title><style>"
    "body { background-color: #f0f0f0; font-family: Arial, sans-serif; }"
    "header { background-color: #333; color: white; padding: 10px; text-align: center; }"
    "h1 { font-size: 2em; margin-top: 20px; }"
    "p { font-size: 1.2em; color: #666; margin: 20px; }"
    "footer { background-color: #333; color: white; padding: 10px; text-align: center; position: fixed; width: 100%; bottom: 0; }"
    "</style></head><body>"
    "<header><h1>Welcome to My Simple C Web Server!</h1></header>"
    "<div><h1>About This Server</h1>"
    "<p>This web server is built entirely using C. It serves content through HTTP, and this page is styled using basic HTML and CSS.</p>"
    "<p>Feel free to add more content, styles, and functionality to make it more interactive and useful!</p>"
    "</div><footer><p>&copy; 2024 Simple C Web Server</p></footer></body></html>";

void handle_client(int client_socket) {
    char buffer[BUF_SIZE];
    int bytes_read = read(client_socket, buffer, BUF_SIZE - 1);

    if (bytes_read < 0) {
        perror("Error reading from socket");
        return;
    }

    buffer[bytes_read] = '\0';

    if (strncmp(buffer, "GET / ", 6) == 0) {
        if (write(client_socket, html_response, strlen(html_response)) < 0) {
            perror("Error writing to socket");
        }
    } else {
        const char *not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n"
                                         "<html><body><h1>404 Not Found</h1></body></html>";
        write(client_socket, not_found_response, strlen(not_found_response));
    }

    shutdown(client_socket, SHUT_RDWR);
    close(client_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Listen on all available interfaces
    address.sin_port = htons(PORT);        // Convert port number to network byte order

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Web server running on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        handle_client(new_socket);
    }

    close(server_fd);
    return 0;
}
