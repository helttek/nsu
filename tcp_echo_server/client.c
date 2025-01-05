#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 50000
#define BUFFER_SIZE 1024

int main() {
  int sockfd;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Connection failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  while (1) {
    printf("Enter message: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    send(sockfd, buffer, strlen(buffer), 0);
    int bytes_read = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (bytes_read <= 0) {
      printf("Server disconnected\n");
      break;
    }
    buffer[bytes_read] = '\0';
    printf("Echo from server: %s", buffer);
  }

  close(sockfd);
  return 0;
}
