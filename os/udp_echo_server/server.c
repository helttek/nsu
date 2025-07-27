#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 50000
#define BUF_SIZE 1024

int main() {
  int sockfd;
  char buffer[BUF_SIZE];
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(client_addr);
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Failed to create a socket in main");
    return EXIT_FAILURE;
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);
  if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("Bind failed in main");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  fprintf(stdout, "UDP server listening\n");
  while (1) {
    int n = recvfrom(sockfd, buffer, BUF_SIZE, MSG_WAITALL,
                     (struct sockaddr *)&client_addr, &addr_len);
    printf("Received from client: %s\n", buffer);
    sendto(sockfd, buffer, n, MSG_CONFIRM,
           (const struct sockaddr *)&client_addr, addr_len);
    printf("Echoed back to client: %s\n", buffer);
  }
  close(sockfd);
  return 0;
}
