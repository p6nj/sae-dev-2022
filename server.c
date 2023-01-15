#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include "common.c"
#include "src/server/fileops.c"
#include "src/server/netops.c"

int main()
{

  int sockfd = sock();
  int clientfd;
  struct sockaddr cliaddr;

  while (wait(sockfd, &cliaddr, &clientfd))
  {
    println("IP%s",
            inet_ntoa(((struct sockaddr_in *)&cliaddr)->sin_addr));

    respond();
    close(clientfd);
    assertquit;
  }

  close(sockfd);
  assertquit;

  return 0;
}