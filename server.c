#include "common.c"
#include "src/server/netops.c"
#include <stdio.h>

int main() {

  int sockfd = sock();
  int clientfd;
  struct sockaddr cliaddr;

  while (wait(sockfd, &cliaddr, &clientfd)) {
    println("IP%s", inet_ntoa(((struct sockaddr_in *)&cliaddr)->sin_addr));

    respond(clientfd);
    close(clientfd);
    assertquit;
  }

  close(sockfd);
  assertquit;

  return 0;
}