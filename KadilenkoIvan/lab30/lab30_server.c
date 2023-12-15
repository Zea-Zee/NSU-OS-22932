#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/some_socket2"

void handle_client(int client_socket) {
    char buffer[256];
    ssize_t bytes_received;

    // Receive data from the client
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("Error receiving data from client");
        exit(EXIT_FAILURE);
    }

    // Convert the received text to uppercase
    for (ssize_t i = 0; i < bytes_received; ++i) {
        buffer[i] = toupper(buffer[i]);
    }

    // Print the converted text to stdout
    printf("Received from client: %s\n", buffer);

    // Close the client socket
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create a Unix domain socket
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Bind the socket to the specified address
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %s\n", SOCKET_PATH);

    // Accept connections and handle clients
    while ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) >= 0) {
        handle_client(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
