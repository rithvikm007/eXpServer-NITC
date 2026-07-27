#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080
#define BUFF_SIZE 10000

typedef struct {
    char message[BUFF_SIZE];
    struct sockaddr_in client_addr;
    int sockfd;
    socklen_t addr_len;
} client_data_t;

void strrev(char *str){
    for(int start=0, end=strlen(str) - 2; start<end ; start++, end--){
        char temp = str[start];
        str[start] = str[end];
        str[end]=temp;
    }
}

void* handle_client(void* arg) {
    client_data_t* data = (client_data_t*)arg;
    printf("[CLIENT MESSAGE] %s",data->message);

    // Reverse the string
    strrev(data->message);

    // Send back the reversed string
    sendto(data->sockfd, data->message, strlen(data->message), 0,(struct sockaddr*)&(data->client_addr), data->addr_len);

    free(data); // Free the allocated memory
    pthread_exit(NULL);
}

int main() {
    int sockfd;
    char buffer[BUFF_SIZE];
    struct sockaddr_in server_addr, client_addr;
    pthread_t thread_id;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);


    // Set server address parameters
    server_addr.sin_family = AF_INET;       // IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
    server_addr.sin_port = htons(PORT);     // Server port

    // Bind the socket to the server address
    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("[INFO] server listening on port %d\n",PORT);

    while (1) {
        socklen_t len = sizeof(client_addr);
        ssize_t n = recvfrom(sockfd, buffer, BUFF_SIZE, 0, (struct sockaddr *)&client_addr, &len);
        buffer[n] = '\0';

        // Allocate memory for client data to pass to the thread
        client_data_t* data = (client_data_t*)malloc(sizeof(client_data_t));
        strcpy(data->message, buffer);
        data->client_addr = client_addr;
        data->sockfd = sockfd;
        data->addr_len = len;

        // Create a new thread to handle the client
        if (pthread_create(&thread_id, NULL, handle_client, (void*)data) != 0) {
            perror("Failed to create thread");
            free(data);
        }

        // Detach the thread to allow independent execution
        pthread_detach(thread_id);
    }

    // Close the socket (unreachable in this infinite loop)
    close(sockfd);
    return 0;
}