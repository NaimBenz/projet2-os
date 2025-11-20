#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "../comm/comm.h"
#include <errno.h>
#include <ctype.h>
#include <signal.h>


char* envIpv4(char* var){
   if(getenv(var) != NULL){
      char* ip = getenv(var);
      struct sockaddr_in sa;
      int result = inet_pton(AF_INET,ip, &(sa.sin_addr));
      if(result == -1){exit(errno);}
      else if(result == 1){return ip;}
   }
   return ADRESSE_IP_PAR_DEFAUT;
}

int envPort(char* var){
   if(getenv(var) != NULL){
      int port = atoi(getenv(var));
      if(isdigit(port) && port>=1 && port <= 65535){return port;}
   } 
   return PORT_PAR_DEFAUT;
}


int main(void) {
   signal(SIGPIPE,SIG_IGN);
   int port = envPort("IP_SERVER");
   char* addr = envIpv4("IP_SERVER");

   int clientSocket = socket(AF_INET,SOCK_STREAM,0);
   struct sockaddr_in addrClient;
   addrClient.sin_addr.s_addr =inet_addr(addr) ;
   addrClient.sin_family = AF_INET;
   addrClient.sin_port = htons(port);

   if (connect(clientSocket,(const struct sockaddr*)&addrClient,sizeof(addrClient))!= 0){exit errno;}

   return 0;
}
