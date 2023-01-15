#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../../common.c"

// Creates and prepares a socket, then returns its file descriptor.
int sock()
{
    int sockfd, client_fd, n, optval;
    unsigned int clilen;
    struct sockaddr_in serv_addr;
    struct sockaddr cli_addr;
    FILE *file;
    char buffer[BUFFER_SIZE], filename[BUFFER_SIZE];
    char mode[2];          // access mode (read or write) for the destination file
    char folder[] = "CSV"; // name of the folder to save or read file
    char name[BUFFER_SIZE];
    struct Result r;

    // Create the server socket
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assertquit;

    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    // Bind the socket to a local address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    assertquit;

    // Listen for n incoming connections
    listen(sockfd, 1);
    assertquit;

    println("Server listening on port %d...\x1B[32m", PORT);

    // Wait for an incoming connection
    clilen = sizeof(serv_addr);

    return sockfd;
}

// Wait for a connection, fill cliaddr with the client address and return 1.
int wait(int sockfd, struct sockaddr *cliaddr)
{
    struct sockaddr_in a; // placeholder for sizeof
    unsigned int clilen = sizeof(a);
    accept(sockfd, cliaddr, &clilen);
    assertquit;
    return 1;
}