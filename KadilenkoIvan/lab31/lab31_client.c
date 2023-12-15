#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/socket3"

int main() {
    int client_socket;
    struct sockaddr_un server_addr;

    // Create a Unix domain socket
    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    // Send data to the server
    char message[] = "Some TEXT fOr server 0_0";
    if (send(client_socket, message, sizeof(message), 0) < 0) {
        perror("Error sending data to server");
        exit(EXIT_FAILURE);
    }

    // Close the client socket
    close(client_socket);

    return 0;
}
