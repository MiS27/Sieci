#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(){

struct sockaddr_in addr;
  struct hostent *host;
  char buf[10];
  host = gethostbyname("nist1.aol-va.symmetricom.com");

  int fd = socket(PF_INET, SOCK_STREAM, 0);
  if(fd){
    addr.sin_family = PF_INET;
    addr.sin_port = htons(13);
    memcpy(&(addr.sin_addr.s_addr), host->h_addr, host->h_length);

    int connection = connect(fd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    int n;
    while ( (n = read(fd, buf, 10)) >0)
	write(1,buf,n);
    close(fd);
  } else {
    printf("Ech...\n");
  }

  return 0;
}
