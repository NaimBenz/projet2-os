#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../comm/comm.h"

/**
 * Exemple de code pour un producteur-consommateur avec
 * un producteur et un consommateur produisant/consommant
 * jusqu'à 5 objets (cf le fichier prod-cons.cpp sur l'UV
 * dans les codes vus en cours).
 **/

#define TAILLE 20

sem_t plein, vide;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

bool isvalid(int n) {
  return (n > 0) && ((n >> 16) == 0);
}
int server(void) {
  const char* stack[20];
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  int opt = 1;

  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
             sizeof(opt));
  struct sockaddr_in adress;
  adress.sin_family = AF_INET;
  adress.sin_addr.s_addr = INADDR_ANY;
  // Setting port for server socket
  if ((getenv("PORT_SERVEUR") != NULL) &&
      isvalid(atoi(getenv("PORT_SERVEUR")))) {
    adress.sin_port =
        inet_pton(AF_INET, getenv("PORT_SERVEUR"), &adress.sin_port);
  } else
    adress.sin_port = PORT_PAR_DEFAUT;

  // Bind socket to server port
  int ret = bind(server_fd, (struct sockaddr*)&adress, sizeof(adress));
  listen(server_fd, 1000);

  size_t addrlen = sizeof(adress);
  // TODO: here i am
  int new_socket =
      accept(server_fd, (struct sockaddr*)&adress, (socklen_t*)&addrlen);
  char buffer[1024];
  int lu;

  // Pour le serveur, on se contente de renvoyer
  // au client tout ce qui est reçu. Comme le
  // socket est SOCK_STREAM, plusieurs appels à
  // read() peuvent être nécessaires pour lire
  // le message en entier.
  // TODO: J'ai un problème avec le fait qu'il y ai un while
  while ((lu = read(new_socket, buffer, 1024)) > 0) {
    if (buffer[0] == 'p') {
      // If producteur, then write on queue
    } else if (buffer[0] == 'c') {
      // If consommateur, then take from queue
    }
  }
  return 0;
}

void* consommateur(void* tampon_) {
  int indice_consommation = 0;
  int nombre_deja_consommes = 0;
  int objet = 0;

  int* tampon = tampon_;

  sem_wait(&plein);
  pthread_mutex_lock(&mutex);
  // Consommer
  objet = tampon[indice_consommation];
  pthread_mutex_unlock(&mutex);
  sem_post(&vide);

  printf("Ici consommateur : tampon[%d] = %d\n", indice_consommation, objet);
  indice_consommation = (indice_consommation + 1) % TAILLE;
  nombre_deja_consommes++;

  return NULL;
}

void* producteur(void* tampon_) {
  int indice_production = 0;
  int nombre_deja_produits = 0;
  int objet;

  int* tampon = tampon_;

  sem_wait(&vide);
  pthread_mutex_lock(&mutex);

  // Produire (objet devrait être mis à jour
  // en fonction de ce qui est produit)
  tampon[indice_production] = objet;
  pthread_mutex_unlock(&mutex);
  sem_post(&plein);
  printf("Ici producteur : tampon[%d] = %d\n", indice_production, objet);
  objet++;
  nombre_deja_produits++;
  indice_production = (indice_production + 1) % TAILLE;
  return NULL;
}

int main(void) {
  pthread_t thread_consommateur;
  pthread_t thread_producteur;

  int tampon[TAILLE];

  sem_init(&plein, 0, 0);
  sem_init(&vide, 0, TAILLE);

  pthread_create(&thread_consommateur, NULL, consommateur, tampon);
  pthread_create(&thread_producteur, NULL, producteur, tampon);

  pthread_join(thread_producteur, NULL);
  pthread_join(thread_consommateur, NULL);
  printf("fin des threads\n");

  return 0;
}
