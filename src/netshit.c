#include "varshit.c"
#include <netinet/in.h>

int suckfd() {
    int sckfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sckfd < 0) return sckfd;
    int optval = 1;
    setsockopt(sckfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    return sckfd;
}

int cast(int sckfd, struct sockaddr_in srvaddr, int port) {
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = INADDR_ANY;
    srvaddr.sin_port = htons(port);
    if (bind(sckfd, (struct sockaddr*) &srvaddr, sizeof(srvaddr)) < 0)
        return false;
    return true;
}