#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
 
struct sockaddr_in addr_in,stClientAddr;
int addr_in_size;
char buff[150];
char servMessage[] = "Witaj nieznajomy!";
char servMessage2[] = "Witaj Kamilu!";
char servMessage3[] = "Witaj Michale!";

void childend(int signo) {
    pid_t pid;
    pid=wait(NULL);
    printf("\t[dzieciak mi umarl %d]\n",pid);
}

struct clientContext {
    int socket;
    struct sockaddr_in addr;
    const char *name;
};

void* clientThread(void* arg) {
    struct clientContext *ctx = (struct clientContext*) arg;
    printf("%s: [connection from %s]\n", ctx->name, inet_ntoa((struct in_addr)ctx->addr.sin_addr));
    read(ctx->socket, &buff, sizeof(buff)); 
    if (!strcmp(buff, "106632")) {
	write(ctx->socket, servMessage2, sizeof(servMessage2));
    } else if (!strcmp(buff, "106643")) {
	write(ctx->socket, servMessage3, sizeof(servMessage3));
    } else {
	write(ctx->socket, servMessage, sizeof(servMessage));
    }
    close(ctx->socket);
    free(ctx);
    pthread_exit(0);
    
}

int main(int argc, char *argv[]) {
    fd_set fsMask, fsRmask, fsWmask;
    static struct timeval tTimeout;
    int nMaxfd, nFound, nFd, nTmp;
    signal(SIGCHLD, childend);
    if (argc < 2) {
	printf("Usage: server server_port");
	return 0;
    }    
    addr_in_size = sizeof(addr_in);
    
    struct hostent* host = gethostbyname("misborg");
    if (!host) {
        perror("gethostbyname");
        exit(-1);
    }
     
    printf("Server listening on port %d\n", atoi(argv[1]));
    addr_in.sin_family = host->h_addrtype; // adres ip na którym ma działać server; jeśli 0.0.0.0 to będzie działał na wszystkim co jest skonfigurowane
    addr_in.sin_port = htons(atoi(argv[1]));  // port na którym będzie słuchal; 
    // porty 1024 są rezerowane dla usług sieciowych, ale root może wszystko ;]
    memcpy(&addr_in.sin_addr.s_addr, host->h_addr, host->h_length);
    int nFoo = 1;
    
    int sd = socket(host->h_addrtype, SOCK_STREAM, 0);
    int nClientSocket;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo));
    if (bind(sd, (struct sockaddr*)&addr_in, sizeof(addr_in)) == -1) { 
      
    // bind może zwrócic -1 gdy stworzymy server, ubijemy i odpalimy znowu
    // przez 2-3 min port jest w stanie gdzie się nie dopuszcza postawienia na nowo servera
        perror("connect (port blocked?)");
        exit(-1);
    }
     
    // na jakim porcie ma słuchać server
    if (listen(sd, 10) == -1) { // drugi argument to rozmiar kolejki oczekiwań (klientów oczekujących) 
        perror("listen");
        exit(-1);
    }


   nMaxfd=sd;
   while(1)
   {
       /* block for connection request -- timeout after 5 sec. */
       FD_SET(sd, &fsRmask);
       fsWmask = fsMask;
       tTimeout.tv_sec = 5;
       tTimeout.tv_usec = 0;
       printf("select...\n");
       nFound = select(nMaxfd + 1, &fsRmask, &fsWmask, (fd_set*) 0, &tTimeout);

       if (nFound < 0)
       {
               fprintf(stderr, "%s: Select error.\n", argv[0]);
       }
       if (nFound == 0)
       {
               printf("%s: Timed out.\n", argv[0]);
               fflush(stdout);
       }
       if (FD_ISSET(sd, &fsRmask))
       {
                nClientSocket = accept(sd, (struct sockaddr*)&stClientAddr, &nTmp);
                if (nClientSocket < 0)
                {
                        fprintf(stderr, "%s: Can't create a connection's socket.\n", argv[0]);
                        exit(1);
                }
                /* connection */
                printf("%s: [connection from %s]\n", argv[0],
                       inet_ntoa((struct in_addr)stClientAddr.sin_addr));
                FD_SET(nClientSocket, &fsMask);
                if (nClientSocket > nMaxfd) nMaxfd = nClientSocket;
       }
       for (nFd = 0; nFd <= nMaxfd; nFd++)
       {
                if (FD_ISSET(nFd, &fsWmask))
                {
                        printf("%s: [sending string to %s]\n", argv[0],
                               inet_ntoa((struct in_addr)stClientAddr.sin_addr));
			read(nFd, &buff, sizeof(buff)); 
			if (!strcmp(buff, "106632")) {
			    write(nFd, servMessage2, sizeof(servMessage2));
			} else if (!strcmp(buff, "106643")) {
			    write(nFd, servMessage3, sizeof(servMessage3));
			} else {
			    write(nFd, servMessage, sizeof(servMessage));
			}
                        FD_CLR(nFd, &fsMask);
                        close(nFd);
                }
       }
   }
 
    close(sd);
    return 0;
}
