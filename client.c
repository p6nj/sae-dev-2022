#include "src/common.c"
#include "src/resparse.c"
#include <netinet/in.h>

int main(int argc, char* argv[]) {
  int sockfd, n;
  struct sockaddr_in servaddr;
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
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // Configuration de l'adresse du serveur
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);

  // Connexion au serveur
  if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
    return throw("connecting to server", 404);

  // Envoi du nom du fichier demandé au serveur
  write(sockfd, req, strlen(req));

  // Réception et affichage du contenu du fichier
  while ((n = read(sockfd, buf, BUFFER_SIZE)) > 0) {
    buf[n] = '\0';
    printf("%s", buf);
  }
  if (n < 0)
    return throw("reading server response", 100);

  // Fermeture de la connexion
  close(sockfd);

  printf("\n");

  return 0;
}
