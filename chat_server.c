#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define Max_clients 10
#define Buffer_size 1024
#define Port 5001

// Define client structure
typedef struct {
    int socket;
    struct sockaddr_in address;
} Client;

Client clients[Max_clients];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to broadcast messages to all clients other than the sender
void broadcast(const char *message, int sender_socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        // As long as the client is not the sender, send the message to them
        if (clients[i].socket != sender_socket) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Function to add client to server
void *handle_client(void *arg) {
    Client client = *(Client *)arg;
    char buffer[Buffer_size];
    char message[Buffer_size + 50];

    while (1) {
        int bytes_received = recv(client.socket, buffer, sizeof(buffer) - 1, 0);
        // Error check
        if (bytes_received <= 0) {
            //Disconnect all clients before closing
            printf("Client disconnected.\n");
            close(client.socket);

            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < client_count; i++) {
                if (clients[i].socket == client.socket) {
                    clients[i] = clients[client_count - 1];
                    client_count--;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
            break;
        }

        // Put message into buffer, null terminator, and broadcast it
        buffer[bytes_received] = '\0';
        snprintf(message, sizeof(message), "Client [%d]: %s", client.socket, buffer);
        printf("%s", message);
        broadcast(message, client.socket);
    }
    return NULL;
}

int main() {
    int server_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to an IP and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(Port);

    // Error check
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen error check
    if (listen(server_socket, Max_clients) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Chat server started. Waiting for clients...\n");

    // Listening for clients
    while (1) {
        // Accept a client connection
        new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        // Error check
        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }

        pthread_mutex_lock(&clients_mutex);
        //Check if there is space to add a client
        if (client_count < Max_clients) {
            //Create a socket for the client and connect them to it
            clients[client_count].socket = new_socket;
            clients[client_count].address = client_addr;
            client_count++;

            pthread_t client_thread;
            pthread_create(&client_thread, NULL, handle_client, (void *)&clients[client_count - 1]);
            pthread_detach(client_thread);

            printf("New client connected: Socket %d\n", new_socket);
        } else {
            // Send client away if no room
            printf("Maximum client limit reached. Connection refused: Socket %d\n", new_socket);
            close(new_socket);
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    // Close server
    close(server_socket);
    return 0;
}