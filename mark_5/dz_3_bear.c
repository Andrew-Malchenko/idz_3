#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 1024

void error(const char *message) {
    perror(message);
    printf("%s", message);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    // Create client socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("Failed to create socket");
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        error("Failed to connect to the server");
    }
    write(client_socket, "bear", strlen("bear"));
    while (1) {
        printf("Bear is sleep\n");
        char response[BUFFER_SIZE];
        memset(response, 0, sizeof(response));
        read(client_socket, response, BUFFER_SIZE);
        sleep(1);

        // Send "collect_honey" message to the server
        write(client_socket, "bear_sleep", strlen("bear_sleep"));
    }

    close(client_socket);

    return 0;
}
