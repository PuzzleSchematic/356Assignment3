#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define Port 5001
#define Buffer_size 1024

// Function to print received messages
void *receive_messages(void *socket) {
    int client_socket = *(int *)socket;
    char buffer[Buffer_size];

    while (1) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        // Error check
        if (bytes_received <= 0) {
            printf("Disconnected from server.\n");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
        // Put message into buffer, end with null terminator, and print it
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    }
    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char message[Buffer_size];

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to chat server.\n");

    // Client always able to type a message while receive messages at the same time.
    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_messages, (void *)&client_socket);

    // Send a message while connected to the server
    while (1) {
        fgets(message, Buffer_size, stdin);
        send(client_socket, message, strlen(message), 0);
    }

    close(client_socket);
    return 0;
}