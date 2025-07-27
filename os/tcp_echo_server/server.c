#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT 50000
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 5

void handle_client(int client_socket) {
  char buffer[BUFFER_SIZE];
  int bytes_read;
  while ((bytes_read = read(client_socket, buffer, sizeof(buffer))) > 0) {
    write(client_socket, buffer, bytes_read);
  }
  close(client_socket);
  exit(0);
}

int main() {
  int server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(client_addr);
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    perror("Socket failed in main");
    exit(EXIT_FAILURE);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);
  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0) {
    perror("Bind failed in main");
    close(server_socket);
    exit(EXIT_FAILURE);
  }
  if (listen(server_socket, MAX_CLIENTS) < 0) {
    perror("Listen failed in main");
    close(server_socket);
    exit(EXIT_FAILURE);
  }
  printf("Server is listening on port %d...\n", PORT);
  while (1) {
    client_socket =
        accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
    if (client_socket < 0) {
      perror("Accept failed in main");
      continue;
    }
    if (fork() == 0) {
      close(server_socket);
      handle_client(client_socket);
    }
    close(client_socket);
  }
  close(server_socket);
  return 0;
}
