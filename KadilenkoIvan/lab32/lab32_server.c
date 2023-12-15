#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/socket4"

void* handle_client(void* arg) {
    int client_socket = *((int*)arg);
    free(arg);

    char buffer[256];
    ssize_t bytes_received;

    // Receive data from the client
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("Error receiving data from client");
    } else if (bytes_received == 0) {
        // Connection closed by the client
        close(client_socket);
        return NULL;
    }

    // Convert the received text to uppercase
    for (ssize_t i = 0; i < bytes_received; ++i) {
        buffer[i] = toupper(buffer[i]);
    }

    // Print the converted text to stdout
    printf("Received from client %d: %s\n", client_socket, buffer);

    // Close the client socket
    close(client_socket);

    return NULL;
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

    while (1) {
        // Accept a new connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }

        // Create a thread to handle the client
        int* arg = malloc(sizeof(int));
        *arg = client_socket;
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, arg) != 0) {
            perror("Error creating thread");
            close(client_socket);
            free(arg);
        }
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
