#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define SERVER_PORT 8080
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 10000
#define NUM_CLIENTS 5

void client_process(int client_id) {
    int client_sock_fd;
    struct sockaddr_in server_addr;

    // Create client socket
    client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock_fd < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(SERVER_PORT);

    // Connect to TCP server
    if (connect(client_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        printf("[Client %d] [ERROR] Failed to connect to server\n", client_id);
        exit(EXIT_FAILURE);
    } else {
        printf("[Client %d] [INFO] Connected to server\n", client_id);
    }

    while (1) {
        // Hardcoded message "hello"
        const char *message = "hello\n";
        size_t message_len = strlen(message);

        // Send hardcoded message to TCP server
        send(client_sock_fd, message, message_len, 0);
        printf("[Client %d] Sent: %s", client_id, message);

        // Buffer to store server's response
        char buff[BUFF_SIZE];
        memset(buff, 0, BUFF_SIZE);

        // Read message from server
        ssize_t read_n = recv(client_sock_fd, buff, sizeof(buff), 0);

        // Client closed connection or error occurred
        if (read_n <= 0) {
            printf("[Client %d] [INFO] Server disconnected. Closing client\n", client_id);
            close(client_sock_fd);
            exit(1);
        }

        // Print message from server
        printf("[Client %d] [SERVER MESSAGE] %s", client_id, buff);

        // Exit after first interaction
        break;
    }

    close(client_sock_fd);
}

int main() {
    pid_t pid;

    // Fork multiple clients
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process runs the client
            client_process(i + 1);
            exit(0);
        }
    }

    // Parent process waits for all child processes to complete
    for (int i = 0; i < NUM_CLIENTS; i++) {
        wait(NULL);
    }

    return 0;
}