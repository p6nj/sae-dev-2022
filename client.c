#include "preferences.c"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  int sockfd;
  struct sockaddr_in servaddr;
  char buf[BUFFER_SIZE];
  int n;

  // Vérification des arguments de la ligne de commande
  if (argc != 2) {
    printf("Usage: %s <file_name>\n", argv[0]);
    exit(1);
  }

  // Création de la socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // Configuration de l'adresse du serveur
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);

  // Connexion au serveur
  connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

  // Envoi du nom du fichier demandé au serveur
  write(sockfd, argv[1], strlen(argv[1]));

  // Réception et affichage du contenu du fichier
  while ((n = read(sockfd, buf, BUFFER_SIZE)) > 0) {
    buf[n] = '\0';
    printf("%s", buf);
  }

  // Fermeture de la connexion
  close(sockfd);

  return 0;
}
