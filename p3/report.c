#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void printUsage();
void sigHandler();

int sig = 0;

int main(int argc, char *argv[]) {
	int num;
	int i;
	char *endptr;
	char buffer;
	int dFlag = 0;
	int kFlag = 0;
	int waitTime = 0;
	int pid;
	char crPID[sizeof(int)+1];
	char tstall[sizeof(int)+1];
	char execSTRs[sizeof(int)+1];
	int toAccessed1[2];
	int toAccessed2[2];
	int accessTotal1[2];
	int accessTotal2[2];
	int totalReport1[2];
	int totalReport2[2];

	/* Argument Checking */

	if (argc < 2) {
		fprintf(stderr, "error: insufficent number of arguments\n");
		printUsage();
	}
	if (argc > 5) {
		fprintf(stderr, "error: too many arguments\n");
		printUsage();
	}
	num = strtol(argv[1], &endptr, 10);
	if (*endptr != '\0') { 
		fprintf(stderr, "error: first argument must be non-zero, positive integer\n");	
		printUsage();
	}
	if (num < 1) {
		fprintf(stderr, "error: first argument must be non-zero, positive integer\n");  
                printUsage();
	}
	if (argc >= 2) {
		for (i = 2; i < argc; i++) {
			if (strcmp(argv[i], "-d") == 0) {
				dFlag = 1;
				i++;
				if ( i >= argc ) {
					fprintf(stderr, "error: -d  argument must be followed by non-zero, positive integer\n");
                                        printUsage();
				}
				waitTime = strtol(argv[i], &endptr, 10);
				if (*endptr != '\0') {
                		fprintf(stderr, "error: -d  argument must be followed by non-zero, positive integer\n");
                		printUsage();
        		}
				if (waitTime <= 0) {
                    fprintf(stderr, "error: -d  argument must be followed by non-zero, positive integer\n");
                    printUsage();
                }		
			} else if (strcmp(argv[i], "-k") == 0) {
				kFlag = 1;
			} else {
				fprintf(stderr, "error: invalid argument\n");
				printUsage();
			}
		}
	}

	/* Set Environment Variables */

	if (kFlag) {
		setenv("UNITS", "k", 1);
	}

	sprintf(tstall, "%d", waitTime);	
	if (dFlag) {
		setenv("TSTALL", tstall, 1);
	}
	
	pid = getpid();
	sprintf(crPID, "%d", pid);
	setenv("TMOM", crPID, 1);

	/* Signal Handler */
	signal(SIGUSR1, sigHandler);

	/* Pipes */

	pipe(toAccessed1);
	pipe(toAccessed2);
	pipe(accessTotal1);
	pipe(accessTotal2);
	pipe(totalReport1);
	pipe(totalReport2);

	/* Write to stdin 
	 * http://stackoverflow.com/questions/15883568/reading-from-stdin */
	while (read(STDIN_FILENO, &buffer, sizeof(buffer)) > 0) {
		write(toAccessed1[1], &buffer, sizeof(buffer));
		write(toAccessed2[1], &buffer, sizeof(buffer));
	}
	close(toAccessed1[1]);
	close(toAccessed2[1]);

	/* Forks 
	 * http://www.unix.com/programming/173811-c-execl-pipes.html 
	 * https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/pipe.html
	 * As we work, close unused pipes as we go for sanity checking purposes. */

	if (pid = fork() == 0) { //accessed 1, pos

		close(toAccessed2[0]); // we don't need these pipes
		close(accessTotal1[0]);
		close(accessTotal2[0]);
		close(accessTotal2[1]);
		close(totalReport1[0]);
		close(totalReport1[1]);
		close(totalReport2[0]);
		close(totalReport2[1]);

		dup2(toAccessed1[0], 0);
		dup2(accessTotal1[1], 1);

		sprintf(execSTRs, "%d", num);
		execl("accessed", "accessed", execSTRs, NULL);
	} else if (pid < 0) {
		fprintf(stderr, "error: failed to create child process: accessed");
	}

	close(toAccessed1[0]);
	close(accessTotal1[1]);

	num = -num;

	if (pid = fork() == 0) { //accessed 2, neg

		close(accessTotal2[0]);
		close(totalReport1[0]);
		close(totalReport1[1]);
		close(totalReport2[0]);
		close(totalReport2[1]);

		dup2(toAccessed2[0], 0);
		dup2(accessTotal2[1], 1);


		sprintf(execSTRs, "%d", num);
		execl("accessed", "accessed", execSTRs, NULL);
	}  else if (pid < 0) {
		fprintf(stderr, "error: failed to create child process: accessed");
	}
	
	close(toAccessed2[0]);
	close(accessTotal2[1]);

	if (pid = fork() == 0) { //total size 1

		close(accessTotal2[0]);
		close(totalReport1[0]);
		close(totalReport2[0]);
		close(totalReport2[1]);

		dup2(accessTotal1[0], 0);
		dup2(totalReport1[1], 1);

		execl("totalsize", "totalsize", NULL);
	} else if (pid < 0) {
		fprintf(stderr, "error: failed to create child process: totalsize");
	}

	close(accessTotal1[0]);
	close(totalReport1[1]);

	if (pid = fork() == 0) { //total size 2
		close(totalReport1[0]);
		close(totalReport2[0]);

		dup2(accessTotal2[0], 0);
		dup2(totalReport2[1], 1);

		execl("totalsize", "totalsize", NULL);
	} else if (pid < 0) {
		fprintf(stderr, "error: failed to create child process: totalsize");
	}

	close(accessTotal2[0]);
	close(totalReport2[1]);

	/* Wait for SIGUSR1 Loop */
	while(sig == 0) {
		sleep(1);
		printf("*");
		fflush(stdout);
	}

	/* Output */
	num = abs(num);

	printf("\n");
	printf("A total of ");
	fflush(stdout);

	while (read(totalReport1[0], &buffer, sizeof(buffer)) > 0) {
		write(STDOUT_FILENO, &buffer, (sizeof(buffer)));
	}
	close(totalReport1[0]);

	if (kFlag == 0) {
		printf("bytes are in regular files not accessed for %d days\n", num);
	} else {
		printf("are in regular files not accessed for %d days\n", num);
	}
	fflush(stdout);
	printf("------------------------------------\n");
	printf("A total of ");
	fflush(stdout);

	while (read(totalReport2[0], &buffer, sizeof(buffer)) > 0) {
		write(STDOUT_FILENO, &buffer, (sizeof(buffer)));
	}

	if (kFlag == 0) {
		printf("bytes are in regular files accessed within %d days\n", num);
	} else {
		printf("are in regular files accessed within %d days\n", num);
	}
	close(totalReport2[0]);
	fflush(stdout);

	exit(0);
}

void printUsage() {
	printf("usage: report [num] {-d seconds} {-k}\n");
	exit(1);	
}

void sigHandler() {
	sig = 1;
}
