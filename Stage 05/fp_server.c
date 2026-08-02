#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFF_SIZE 10000
#define MAX_ACCEPT_BACKLOG 5

char buffer[BUFF_SIZE];

void write_to_file(int conn_sock_fd) {
    char buffer[BUFF_SIZE];
    ssize_t bytes_received;
    
    // Open the file to which the data from the client is being written
    FILE *fp;
    const char *filename="t2.txt";
    fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("[-]Error in creating file");
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Receiving data from client...\n");
    while ((bytes_received = recv(conn_sock_fd, buffer, sizeof(buffer), 0)) > 0) {
        printf("[FILE DATA] %s", buffer); // Print received data to the console
        fprintf(fp, "%s", buffer);      // Write data to file
        memset(buffer, 0, BUFF_SIZE);   // Clear the buffer
    }

    if (bytes_received < 0) {
        perror("[-]Error in receiving data");
    }
    fclose(fp);
    printf("[INFO] Data written to file successfully.\n");
}

int main() {
    // Creating listening sock
    int listen_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Setting sock opt reuse addr
    int enable = 1;
    setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    // Creating an object of struct socketaddr_in
    struct sockaddr_in server_addr;

    // Setting up server addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Binding listening sock to port
    bind(listen_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Starting to listen
    listen(listen_sock_fd, MAX_ACCEPT_BACKLOG);
    printf("[INFO] Server listening on port %d\n", PORT);

    // Creating an object of struct socketaddr_in
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    // Accept client connection
    int conn_sock_fd = accept(listen_sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    printf("[INFO] Client connected to server\n");

    write_to_file(conn_sock_fd);
    close(conn_sock_fd);
    return 0;
}