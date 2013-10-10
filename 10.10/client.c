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
  struct hostent *host;
  char index[]="106643";
  char buf[10];
  host = gethostbyname(argv[1]);

  int fd = socket(PF_INET, SOCK_STREAM, 0);
  if(fd){
    addr.sin_family = PF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    memcpy(&(addr.sin_addr.s_addr), host->h_addr, host->h_length);

    int connection = connect(fd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    int n;
    write(fd,index,sizeof(index));
    while ( (n = read(fd, buf, 10)) >0)
	write(1,buf,n);
    close(fd);
  } else {
    printf("Ech...\n");
  }

  return 0;
}
