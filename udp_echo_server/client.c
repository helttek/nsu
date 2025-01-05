#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 50000
#define BUFFER_SIZE 1024

int main() {
  int sockfd;
  char buffer[BUFFER_SIZE];
  struct sockaddr_in server_addr;
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost
  while (1) {
    printf("Enter message: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM,
           (const struct sockaddr *)&server_addr, sizeof(server_addr));
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, MSG_WAITALL, NULL, NULL);
    buffer[n] = '\0';
    printf("Echo from server: %s\n", buffer);
  }
  close(sockfd);
  return 0;
}
