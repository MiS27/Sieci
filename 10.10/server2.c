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
 
struct sockaddr_in addr_in;
int addr_in_size;
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
    char buff[150];
    struct clientContext *ctx = (struct clientContext*) arg;
    printf("%s: [connection from %s]\n", ctx->name, inet_ntoa((struct in_addr)ctx->addr.sin_addr));
    int place=0;
    while (1) {
	place+=read(ctx->socket, &buff[place], sizeof(buff)-place); 
	if(strchr(buff,'\n')!=NULL)
		break;
	}
        printf("%s\n",buff);
    if (!strcmp(buff, "106632\n")) {
	place=0;
	int tmp=0;
	while (1) {
		tmp=write(ctx->socket, &servMessage2[place], sizeof(servMessage2)-place);
		place+=tmp;
		if(tmp==0)
			break;
	}
    } else if (!strcmp(buff, "106643\n")) {
        write(ctx->socket, servMessage3, sizeof(servMessage3));
    } else {
	place=0;
	int tmp=0;
	while (1) {
		tmp=write(ctx->socket, &servMessage[place], sizeof(servMessage)-place);
		place+=tmp;
		if(tmp==0)
			break;
	}
    }
	place=0;
    while (1) {
	place+=read(ctx->socket, &buff[place], sizeof(buff)-place); 
	if(strchr(buff,'\n')!=NULL)
		break;
	}
    close(ctx->socket);
    free(ctx);
    pthread_exit(0);
    
}

int main(int argc, char *argv[]) {
    signal(SIGCHLD, childend);
    if (argc < 2) {
        printf("Usage: server server_port");
        return 0;
    }    
    addr_in_size = sizeof(addr_in);
    
    struct hostent* host = gethostbyname("lab-net-10");
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
    //
    //
    int i=0;
    while (1) {
        i++;
        struct clientContext *ctx = malloc(sizeof(struct clientContext));
        socklen_t nTmp = sizeof(ctx->addr);
        ctx->name = argv[0];
        ctx->socket = accept(sd, (struct sockaddr*)&addr_in, &addr_in_size); // zwraca client descriptor
        if (ctx->socket < 0) {
            printf(stderr, "Nie umiem tworzyc polaczen\n");
            exit(1);
        }
        pthread_t id;
        pthread_create(&id, NULL, clientThread, ctx);
            printf("%d\n",i);
        /*read(csd, &buff, sizeof(buff)); 
        if (!fork()) {
            if (!strcmp(buff, "106632")) {
                write(csd, servMessage2, sizeof(servMessage2));
            } else if (!strcmp(buff, "106643")) {
                write(csd, servMessage3, sizeof(servMessage3));
            } else {
                write(csd, servMessage, sizeof(servMessage));
            }
            close(csd);
            close(sd);
            exit(0);
         }
         close(csd);
         */
   }
// ostatni argument to adres na zmienna z sizeof na strukture)
//     read(sd, &buff, sizeof(buff)); 
//     printf("%s\n", buff);
 
    close(sd);
    return 0;
}
