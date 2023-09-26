#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8888
#define MAX_CLIENTS 3
#define MAX_THREADS 1
#define BUFFER_SIZE 1024

typedef struct {
    int honey_level;
    int bear_sleeping;
    pthread_mutex_t mutex;
} HoneyPot;

typedef struct {
    int bee_socket;
    int bear_socket;
    int viewer_socket;
    int honey_pot_limit;
    HoneyPot *honey_pot;
} ClientData;

void error(const char *message) {
    perror(message);
    printf("%s", message);
    exit(EXIT_FAILURE);
}

void eats_honey(int bear_socket, int viewer_socket){
    printf("Bear is eating honey\n");
    write(bear_socket, "up_bear", strlen("up_bear"));
    write(viewer_socket, "bear up", strlen("bear up"));
    char response[BUFFER_SIZE];
    while (1) {
        memset(response, 0, sizeof(response));
        read(bear_socket, response, BUFFER_SIZE);
        if(strcmp(response, "bear_sleep") == 0){
           break;
        }
        printf("Incorrect response: %s\n", response);
    }
    write(viewer_socket, "bear sleep", strlen("bear sleep"));
}

void* handle_client(void *arg) {
    ClientData *client_data = (ClientData*)arg;
    int client_socket = client_data->bee_socket;
    int bear_socket = client_data->bear_socket;
    int viewer_socket = client_data->viewer_socket;
    int honey_pot_limit = client_data->honey_pot_limit;
    HoneyPot *honey_pot = client_data->honey_pot;
    char response[BUFFER_SIZE];
    int read_size;

    while (1) {
        memset(response, 0, sizeof(response));
        read(client_socket, response, BUFFER_SIZE);

            pthread_mutex_lock(&(honey_pot->mutex));

            honey_pot->honey_level++;
            printf("Bee number %d added honey to the pot.\n", atoi(response));
            write(viewer_socket, "Bee added honey to the pot.", strlen("Bee added honey to the pot."));

            if (honey_pot->honey_level == honey_pot_limit) {
                honey_pot->bear_sleeping = 0;
                printf("The honey pot is full!\n");
                write(viewer_socket, "The honey pot is full!", strlen("The honey pot is full!"));
                eats_honey(bear_socket, viewer_socket);
                printf("Bear is sleep\n");
                honey_pot->honey_level = 0;
                honey_pot->bear_sleeping = 1;
            }

            pthread_mutex_unlock(&(honey_pot->mutex));
    }

    close(client_socket);
    close(bear_socket);
}

int main(int argc, char **argv) {
    int server_socket, client_socket, opt = 1, count_bee = atoi(argv[4]);
    struct sockaddr_in server_address, client_address;
    HoneyPot honey_pot = { 0, 10, PTHREAD_MUTEX_INITIALIZER };
    pthread_t threads[MAX_THREADS];

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("Failed to create socket");
    }

    // Set socket options
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        error("Failed to set socket options");
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));

    // Bind the socket to the specified address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        error("Failed to bind");
    }

    // Start listening for incoming connections
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        error("Failed to listen");
    }

    printf("Server started. Waiting for clients...\n");

    int bee_socket, bear_socket, viewer_socket;

    // Accept client connections
    int address_size = sizeof(client_address);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&address_size)) < 0) {
            error("Failed to accept client connection");
        }

        char response[BUFFER_SIZE];
        memset(response, 0, sizeof(response));
        read(client_socket, response, BUFFER_SIZE);
        if (strcmp(response, "bee") == 0) {
            printf("Client bee connected. Waiting for honey collection...\n");
            bee_socket = client_socket;
        } else {
          if (strcmp(response, "bear") == 0) {
            printf("Client bear connected. Waiting for honey collection...\n");
            bear_socket = client_socket;
          } else {
            if (strcmp(response, "viewer") == 0) {
                printf("Client viewer connected. Waiting for honey collection...\n");
                viewer_socket = client_socket;
            } else {
                printf("Client %s connected. Waiting for honey collection...\n", response);
            }
          }
        }
    }
    ClientData *client_data = (ClientData*)malloc(sizeof(ClientData));

    client_data->honey_pot = &honey_pot;
    client_data->bee_socket = bee_socket;
    client_data->bear_socket = bear_socket;
    client_data->viewer_socket = viewer_socket;
    client_data->honey_pot_limit = atoi(argv[3]);
    
    char str[BUFFER_SIZE];
    snprintf(str, BUFFER_SIZE - 1, "%d", count_bee);
    write(bee_socket, str, strlen(str));
    
    if (pthread_create(&threads[0], NULL, handle_client, (void*)client_data) != 0) {
        error("Failed to create thread");
    }
    // Wait for all threads to finish
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
