#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "../comm/comm.h"

/**
 * Exemple de code pour un producteur-consommateur avec
 * un producteur et un consommateur produisant/consommant
 * jusqu'à 5 objets (cf le fichier prod-cons.cpp sur l'UV
 * dans les codes vus en cours).
 **/

#define TAILLE 3
#define STDERR_FD 2


sem_t plein, vide;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* consommateur(void* tampon_) {
  int indice_consommation = 0;
  int nombre_deja_consommes = 0;
  int objet = 0;

  int* tampon = tampon_;

  do {
    sem_wait(&plein);
    pthread_mutex_lock(&mutex);
    // Consommer
    objet = tampon[indice_consommation];
    pthread_mutex_unlock(&mutex);
    sem_post(&vide);

    printf("Ici consommateur : tampon[%d] = %d\n", indice_consommation, objet);
    indice_consommation = (indice_consommation + 1) % TAILLE;
    nombre_deja_consommes++;
    sleep(2);
  } while (nombre_deja_consommes <= 5);

  return NULL;
}

void* producteur(void* tampon_) {
  int indice_production = 0;
  int nombre_deja_produits = 0;
  int objet;

  int* tampon = tampon_;

  do {
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
    sleep(2);
  } while (nombre_deja_produits <= 5);
  return NULL;
}

//Faire un vector de string afin de limiter la répétition de variable


void s_sigpipe(){
    const char message[]= "Interception of sigpipe";
    size_t number_character = strlen(message);
    ssize_t n = write(STDERR_FD,message,number_character);
    (void)n;
}

void s_sigint(){
    const char message[]= "Interception of %i";
    size_t number_character = strlen(message);
    ssize_t n = write(STDERR_FD,message,number_character);
    (void)n;
}


void handler(int signal){
  switch (signal)
  {
  case SIGINT:
    s_sigint();
    break;
  case SIGPIPE:
    s_sigpipe();
    break;
  }
}

int main(void) {

  struct sigaction action;
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);  
  action.sa_flags = 0;               

  if (sigaction(SIGPIPE, &action, NULL) < 0
        || sigaction(SIGINT,&action,NULL) < 0) {
        perror("sigaction");
  }

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
