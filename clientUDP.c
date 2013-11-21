#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>

int main(int argc, char*argv[]){

  struct sockaddr_in addr;
  struct sockaddr_in serverAddr;
  struct hostent *host;
  char index[]="106643";
  char buf[100];
  host = gethostbyname(argv[1]);

  int fd = socket(PF_INET, SOCK_DGRAM, 0);
  if(fd){
    addr.sin_family = PF_INET;
    addr.sin_port = 0;
    addr.sin_addr.s_addr=INADDR_ANY;
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) { 
      
    // bind może zwrócic -1 gdy stworzymy server, ubijemy i odpalimy znowu
    // przez 2-3 min port jest w stanie gdzie się nie dopuszcza postawienia na nowo servera
        perror("bind problem");
        exit(-1);
    }
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    memcpy(&(serverAddr.sin_addr.s_addr), host->h_addr, host->h_length);
    sendto(fd,index,sizeof(index),0,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    printf("wyslalem\n");
    int n=recv(fd,buf,sizeof(buf),0);
    write(1,buf,n);
    close(fd);
  } else {
    printf("Ech...\n");
  }

  return 0;
}
