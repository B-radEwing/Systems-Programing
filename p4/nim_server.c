#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef struct Node Node;
typedef struct List List;

struct Node {
    char *player1;
    char *player2;
    int p1connection;
    int p2connection;
    Node *next;
};

void printUsage();
void sigHandler();
void daemonFunc();
void stream();

int sig = 0;
int pMode = 0;
int sockFD;
char *p1 = "";
char *p2 = "";
char connec1[sizeof(int)+1];
char connec2[sizeof(int)+1];

int main(int argc, char *argv[]) {
	
	if (argc > 1) {
		printf("error: too many arguments\n");
		printUsage();
	}	

	// Signal Handler
	signal(SIGUSR2, sigHandler);

	stream();
	daemonFunc();

	exit(0);
}

void stream() {
	struct addrinfo hints, *addrlist;
	FILE *fd;
	struct sockaddr_in *localaddress;
	socklen_t sockLength;
	char myhostname[128];

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_NUMERICSERV | AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_next = NULL; hints.ai_addr = NULL;
	hints.ai_canonname = NULL;

	gethostname(myhostname, 128);
	getaddrinfo(NULL, "0", &hints, &addrlist);	
	localaddress = (struct sockaddr_in *) addrlist->ai_addr;
	sockFD = socket(addrlist->ai_family, addrlist->ai_socktype, 0);
	bind(sockFD, (struct sockaddr *) localaddress, sizeof(struct sockaddr_in));
	sockLength = sizeof(struct sockaddr_in);
	getsockname(sockFD, (struct sockaddr *) localaddress, &sockLength);
	
	fd = fopen("control.txt" , "w");
	fprintf(fd, "%s\n", myhostname);
	fprintf(fd, "%d\n", ntohs(localaddress->sin_port));
	fclose(fd);	

	listen(sockFD, 1);
	printf("Socket fd: %d\n", sockFD);
}

void daemonFunc() {
	char handles[22];
	struct sockaddr_in from;
	struct sockaddr_in client;
	struct sockaddr_in tcpIN;
	socklen_t fsize; socklen_t client_length; socklen_t tcpIN_length;
	fsize = sizeof(from);
	client_length = sizeof(client);
	tcpIN_length = sizeof(tcpIN);
	int conn; int pid;
	fd_set mask;
	struct timeval wait;	

	while (1) {
		
		FD_ZERO(&mask);
		FD_SET(sockFD, &mask);
		wait.tv_sec = 1;
		wait.tv_usec = 0;

		select(sockFD+1, &mask, (fd_set *) 0, (fd_set *) 0, &wait);		

		if (FD_ISSET(sockFD, &mask)) {
			conn = accept(sockFD, (struct sockaddr *) &client, &client_length);
				
			recvfrom(conn, handles, sizeof(handles), 0, (struct sockaddr *) &tcpIN, &tcpIN_length);
			if(p1 == "") {
				p1 = malloc(sizeof(handles));
				strcpy(p1, handles);
				sprintf(connec1, "%d", conn);
			} else {
				p2 = malloc(sizeof(handles));
				strcpy(p2, handles);
				sprintf(connec2, "%d", conn);
				pid = fork();
				if (!pid) { // child
					execl("nim_match_server", "nim_match_server", p1, connec1, p2, connec2, NULL);
					exit(1);
				} else { // parent
					exit(0);
				}

				

			}
		}	
	}
}

void sigHandler() {
	remove("control.txt");
}

void printUsage() {
	printf("usage: nim_server {XXXXXX}");
	exit(1);
}
