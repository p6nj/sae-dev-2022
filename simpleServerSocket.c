/* simpleServerSocket.c */
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int sockfd, newsockfd, portno;
  unsigned int clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;

  /* First call to socket() function */
  sockfd = socket(PF_INET, SOCK_STREAM, 0);

  /* Initialize socket structure */
  // bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = 5001;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* Now bind the host address using bind() call.*/
  bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  /* Now start listening for the clients, here process will
   * go in sleep mode and will wait for the incoming connection
   */

  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  int i = 3;

  while (1) {
    /* Accept actual connection from the client */
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

    /* If connection is established then start communicating */
    bzero(buffer, 256); // met le buffer à zéro (identique à memset)
    n = read(newsockfd, buffer, 255);

    printf("Here is the message: %s", buffer);

    /* Write a response to the client */
    n = write(newsockfd, "data goes here", 18);
  }

  return 0;
}