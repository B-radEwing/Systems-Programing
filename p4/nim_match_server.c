#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <poll.h>
#include <string.h>

void sendBoard(int connection);

int row1 = 1; int row2 = 3; int row3 = 5; int row4 = 7;

int main(int argc, char *argv[]) {
	char *player1;
	char *player2;
	int p1connection;
	int p2connection;
	struct pollfd ufds[2];
	char *endptr;
	int check;

	player1 = argv[1];
	p1connection = strtol(argv[2], &endptr, 10);
	player2 = argv[3];
	p2connection = strtol(argv[4], &endptr, 10);

	write(p1connection, player2, strlen(player2)+1);
	write(p2connection, player1, strlen(player1)+1);

	sendBoard(p1connection);

	while(1) {
		fd_set mask;
		int highFD;

		if (p1connection > p2connection) {
			highFD = p1connection;
		} else {
			highFD = p2connection;
		}

		FD_ZERO(&mask);
		FD_SET(p1connection, &mask);
		FD_SET(p2connection, &mask);

		select(highFD+1, &mask, (fd_set *) 0, (fd_set* ) 0 , NULL);

		if(FD_ISSET(p1connection, &mask)) {
			read(p1connection, &row1, sizeof(row1));
			read(p1connection, &row2, sizeof(row2));
			read(p1connection, &row3, sizeof(row3));
			read(p1connection, &row4, sizeof(row4));
			if (check < 0) {
				row1 = -1;
				sendBoard(p2connection);
				exit(1);
			}
			sendBoard(p2connection);
		} else if(FD_ISSET(p2connection, &mask)) {
			read(p2connection, &row1, sizeof(row1));
                        read(p2connection, &row2, sizeof(row2));
                        read(p2connection, &row3, sizeof(row3));
                        read(p2connection, &row4, sizeof(row4));
                        if (check < 0) {
                                row1 = -1;
                                sendBoard(p1connection);
                                exit(1);
                        }
			sendBoard(p1connection);
		}
		if ( (row1 == 0) && (row2 == 0) && (row3 == 0) && (row4 == 0)) {
                                exit(0);
		}
	}
	exit(0);
}

void sendBoard(int connection) {
    write(connection, &row1, sizeof(row1));
    write(connection, &row2, sizeof(row2));
    write(connection, &row3, sizeof(row3));
    write(connection, &row4, sizeof(row4));
}
