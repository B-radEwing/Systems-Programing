#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>

void printUsage();
void ports();
void printBoard();
void flushInput();
void gameMode();

char tcpPort[64];
char server[64];
int pMode = 0;

// the nim board
int row1 = 0; int row2 = 0; int row3 = 0; int row4 = 0;

int main(int argc, char *argv[]) {

	if (argc > 1) {
		printf("error: too many arguments\n");
		printUsage();
	}
	
	ports();
	gameMode();

	exit(0);
}

void ports() {
	FILE *fd;
	fd = fopen("control.txt", "r");
	if (fd == NULL) {
		printf("error: control file was not opened correctly.\n");
		exit(1);
	}

	fgets(server, 64, fd);
	fgets(tcpPort, 64, fd);

	server[strlen(server)-1] = '\0';
	tcpPort[strlen(tcpPort)-1] = '\0';
	fclose(fd);
}

void gameMode() {

	int row; int col;
	int fd;
	char c;
	char *endptr;
	struct sockaddr_in *serverStruct;
	struct addrinfo hints, *addrlist;
	char handle[21];
	int game = 1;
	int validMove = 0;
	
	/* Get Game */
	printf("Please enter your handle: \n");
	fgets(handle, sizeof(handle), stdin);

	int handle_length = strlen(handle) - 1;
	handle[handle_length] = '\0';

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_NUMERICSERV;
	hints.ai_protocol = 0;
	hints.ai_next = NULL; hints.ai_addr = NULL;
	hints.ai_canonname = NULL;

	getaddrinfo(server, tcpPort, &hints, &addrlist);	
	serverStruct = (struct sockaddr_in *) addrlist->ai_addr;
	fd = socket(addrlist->ai_family, addrlist->ai_socktype, 0);
	connect(fd, (struct sockaddr *) serverStruct, sizeof(struct sockaddr_in));

	write(fd, handle, sizeof(handle));
	printf("Connecting to an opponent.\n");
	printf("Opponent: ");

	while((read(fd, &c, 1) == 1) && (c != '\0')) {
		putchar(c);
	} 
	// game
	printf("\n");

	while (game) {
		fd_set mask;
		FD_ZERO(&mask);
		FD_SET(fd, &mask);

		select(fd+1, &mask, NULL, NULL, NULL);

		if(FD_ISSET(fd, &mask)) {
			read(fd, &row1, sizeof(row1));
			read(fd, &row2, sizeof(row2));
			read(fd, &row3, sizeof(row3));
			read(fd, &row4, sizeof(row4));
			if (row1 == -1) {
				printf("The connection to your opponent was lost!\n");
				exit(0);
			}
			if ( (row1 == 0) && (row2 == 0) && (row3 == 0) && (row4 == 0)) {
				printBoard();
				printf("You win!\n");
				exit(0);
			}
			printBoard();
			validMove = 0;
			while(!validMove) {
				printf("Move: ");
				scanf("%s%s", &row, &col);
				row = strtol(&row, &endptr, 10);
				if(*endptr != '\0') {
					printf("sfirst argument incorrect: must be valid positive integer.\n");
					flushInput();
					continue;
				}
				col = strtol(&col, &endptr, 10);
				if(*endptr != '\0') {
					printf("second argument incorrect: must be valid positive integer.\n");
					flushInput();
					continue;
				}

					// resignation move
				if((row == 0) && (col == 0)) {
					validMove = 1;
					printf("You resign!\n");
					game = 0;
					row1 = -1;
				}

				if ((row < 0) || (row > 4)) {
					printf("Invalid row selection.\n");
					continue;
				}
				if ((col < 1) || (col > 7)) {
					printf("Invalid number of stones to remove\n");
					continue;
				}

				switch(row) {
					case 1:
						if (col <= row1) {
							row1 -= col;
							validMove = 1;
						} else {
							printf("error: cannot remove more stones than present\n");
						}
						break;
					case 2:
						if (col <= row2) {
							row2 -= col;
							validMove = 1;
						} else {
							printf("error: cannot remove more stones than present\n");
						}
						break;
					case 3:
						if (col <= row3) {
							row3 -= col;
							validMove = 1;
						} else {
							printf("error: cannot remove more stones than present\n");
						}
						break;
					case 4:
						if (col <= row4) {
							row4 -= col;
							validMove = 1;
						} else {
							printf("error: cannot remove more stones than present\n");
						}
						break;
					default:
						printf("Invalid row\n"); //execution should not reach
				}
			}
			write(fd, &row1, sizeof(row1));
			write(fd, &row2, sizeof(row2));
			write(fd, &row3, sizeof(row3));
			write(fd, &row4, sizeof(row4));	
			
			printBoard();	
			if ( (row1 == 0) && (row2 == 0) && (row3 == 0) && (row4 == 0)) {
                                printBoard();
                                printf("You lose!\n");
                                exit(0);		
			}
		}
	}
}

void printBoard() {
	int i;

	printf("1| ");
	for(i = 0; i < row1; i++) {
		printf("O ");
	}
	printf("\n2| ");
	for(i = 0; i < row2; i++) {
		printf("O ");
	}
	printf("\n3| ");
	for(i = 0; i < row3; i++) {
		printf("O ");
	}
	printf("\n4| ");
	for(i = 0; i < row4; i++) {
		printf("O ");
	}
	printf("\n +");
	for (i = 0; i < 20; i++) {
		printf("-");
	}
	printf("\n   ");
	i = 1;
	while (i < 8) {
		printf("%d ", i);
		i++;
	}
	printf("\n");
}

void flushInput() {
	int c;
	while((c = getchar()) != '\n' && c != EOF);
}

void printUsage() {
	printf("usage: ./nim\n");
	exit(1);
}
