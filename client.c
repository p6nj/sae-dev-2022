#include "src/common.c"
#include "src/resparse.c"
#include "src/netshit.c"

int main(int argc, char* argv[]) {
  int sockfd, n, servfd;
  struct sockaddr_in servaddr;
  struct sockaddr_in selfaddr;
  struct sockaddr someaddr;
  unsigned int iplen = sizeof(someaddr);
  char buf[BUFFER_SIZE];
  char req[TOTALMAX];

  // Vérification des arguments de la ligne de commande
  if (argc != 2) {
    printf("Usage: %s <r/w><filename(max:%d)>[|<data(max:%d)>]\n", argv[0], MAXREQUESTSIZE - 1, MAXFILESIZE);
    return EXIT_FAILURE;
  }
  if (strlen(argv[1]) > TOTALMAX - 1) { // too big for the req string!
    printf("Usage: %s <r/w><filename(max:%d)>[|<data(max:%d)>]\n", argv[0], MAXREQUESTSIZE - 1, MAXFILESIZE);
    return EXIT_FAILURE;
  }
  strcpy(req, argv[1]);
  if (!response_ok(req)) {
    printf("Usage: %s <r/w><filename(max:%d)>[|<data(max:%d)>]\n", argv[0], MAXREQUESTSIZE - 1, MAXFILESIZE);
    return EXIT_FAILURE;
  }


  // Création de la socket
  if ((sockfd = suckfd()) < 0)
    return throw("creating socket", 1);

  // Configuration de l'adresse du serveur
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);

  // Connexion au serveur
  if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
    return throw("connecting to server", 404);

  for (int i = 0; i < strlen(req); i += BUFFER_SIZE) {
    char buf[BUFFER_SIZE + 1];
    strncpy(buf, req + i, BUFFER_SIZE);
    buf[BUFFER_SIZE] = '\0';
    send(sockfd, buf, strlen(buf), 0);
  }

  close(sockfd);
  // Création de la socket
  if ((sockfd = suckfd()) < 0)
    return throw("creating socket", 1);

  // Bind the socket to a local address
  // !!!!!!!!!! IF YOU GET AN ERROR HERE CHECK YOUR PORT NUMBER !!!!!!!!!!!!!!
  if (!cast(sockfd, selfaddr, RESPORT))
    return throw("binding socket", 2);

  // Listen for n incoming connections
  if (listen(sockfd, 1) < 0)
    return throw("listening for connections", 3);

  servfd = accept(sockfd, &someaddr, &iplen);
  if (servfd < 0)
    return throw("accepting connection", 4);

  // Réception et affichage du contenu du fichier
  while ((n = read(servfd, buf, BUFFER_SIZE)) > 0) {
    buf[n] = '\0';
    printf("%s", buf);
  }
  if (n < 0)
    return throw("reading server response", 100);

  // Fermeture de la connexion
  close(servfd);
  close(sockfd);

  printf("\n");

  return 0;
}
