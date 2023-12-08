#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>

#define SOCKET_PATH "/tmp/socket3"

void handle_client(int client_socket) {
    char buffer[256];
    ssize_t bytes_received;

    // Receive data from the client
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("Error receiving data from client");
        exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
        // Connection closed by the client
        close(client_socket);
        return;
    }

    // Convert the received text to uppercase
    for (ssize_t i = 0; i < bytes_received; ++i) {
        buffer[i] = toupper(buffer[i]);
    }

    // Print the converted text to stdout
    printf("Received from client %d: %s\n", client_socket, buffer);

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

    fd_set read_fds;
    int max_fd;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);
        max_fd = server_socket;

        // Wait for input from either a new client or the user
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Error in select");
            exit(EXIT_FAILURE);
        }

        // Check if a new connection is available
        if (FD_ISSET(server_socket, &read_fds)) {
            client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
            if (client_socket < 0) {
                perror("Error accepting connection");
                exit(EXIT_FAILURE);
            }

            // Add the new client socket to the set
            FD_SET(client_socket, &read_fds);
            if (client_socket > max_fd) {
                max_fd = client_socket;
            }

            printf("New connection from client %d\n", client_socket);
        }

        // Check if any client socket has data to read
        for (int i = server_socket + 1; i <= max_fd; ++i) {
            if (FD_ISSET(i, &read_fds)) {
                handle_client(i);
            }
        }

        // Check for user input to exit the loop
        printf("Enter 0 to exit: ");
        char input[2];
        fgets(input, sizeof(input), stdin);

        if (input[0] == '0') {
            break; // Exit the loop
        }
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
