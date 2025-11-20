#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "../comm/comm.h"
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include<stdio.h>
#include <pthread.h>


char* envToIpv4(char* var)
/* Retourne la bonne ipv4 en fonction d'une variable d'environnement */
{ 
   if(getenv(var) != NULL){
      char* ip = getenv(var);
      struct sockaddr_in sa;
      int check = inet_pton(AF_INET,ip, &(sa.sin_addr)); //vérifie si le string est au format d'un ipv4
      if(check == -1){exit(errno);}
      else if(check == 1){return ip;}
   }
   return ADRESSE_IP_PAR_DEFAUT;
}

int envToPort(char* var)
/* Retourne le bon port en fonction d'une variable d'environnement */
{
   if(getenv(var) != NULL){
      int port = atoi(getenv(var));
      if(isdigit(port) && port>=1 && port <= 65535){return port;}
   } 
   return PORT_PAR_DEFAUT;
}

void* consomme(void *){}

void produit(){}

void boucle(){
   char* commande;
   int loop = 1;
   int getret;
   size_t size;
   while(loop){
      fprintf(stdout,">>> ");
      fflush(stdout);
      if ((getret = getline(&commande,&size,stdin)) == EOF){ // lit sur stdin
         printf(stderr,"\n");
         fflush(stderr);
         exit errno;
      }
      else if(getret == -1){
         perror("Error reading input\n");
         exit errno;
      }
      commande[strcspn(commande,"\n")] = '\0'; //rajoute le carractere '\0' à la fin de la chaine de carractere
      
      if (strncmp(commande,"c",1) == 0){
         pthread_t thread;

         if (pthread_create(&thread,NULL,consomme,NULL) != 0){ //thread create
            fprintf(stderr,"Erreur thread\n");
            fflush(stderr);
            exit(EXIT_FAILURE);
         } 

        if (pthread_join(thread,NULL)!=0){ //thread join
            fprintf(stderr,"Erreur Thread\n");
            fflush(stderr);
            exit(EXIT_FAILURE);
         }
      }
      else if(strncmp(commande,"p",1) == 0 ){produit();}

      else{
         fprintf(stderr,"Commande inconnue.\n");
         fflush(stderr);
      }
   }
}


int main(void) {
   signal(SIGPIPE,SIG_IGN);
   int port = envToPort("IP_SERVER"); //met le bon port dans la variable en int
   char* addr = envToIpv4("IP_SERVER"); //met l'addresse ip en string dans la variable

   int clientSocket = socket(AF_INET,SOCK_STREAM,0);
   struct sockaddr_in addrClient;
   addrClient.sin_addr.s_addr =inet_addr(addr) ;
   addrClient.sin_family = AF_INET;
   addrClient.sin_port = htons(port);

   if (connect(clientSocket,(const struct sockaddr*)&addrClient,sizeof(addrClient))!= 0){exit errno;}

   close(clientSocket);
   return 0;
}
