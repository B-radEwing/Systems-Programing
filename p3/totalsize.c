#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

typedef struct Node Node;

struct Node {
    long ino;
    Node *next;
};

int main(int argc, char *argv[]) {
	int size;
	long inode;
	struct stat buf;
	char files[4096];
	char *endptr;
	Node *head = NULL;
	Node *temp;
	int repeat = 0;
	int stall = 0;
	int pid;

	if (getenv("TSTALL") != NULL) {
		stall = strtol(getenv("TSTALL"), &endptr, 10);
		if (*endptr != '\0') {
	    	stall = 0;
		}
		if (stall <= 0) {
			stall = 0;
		}

		if (stall > 0) {
			sleep(stall);
		}
	}
	// parse stdin	
 	while(scanf("%s", files) != EOF) {	
		repeat = 0;
		if (!stat(files, &buf)) {
			if (!S_ISREG(buf.st_mode)) {
				continue; // not regular file
			}
			inode = (long) buf.st_ino;

			Node *newNode = malloc(sizeof(Node));
			newNode->ino = inode;
			newNode->next = NULL;

			if (head == NULL) { // first insertion case
				head = newNode;
			} else {
				temp = head;
				while (temp->next != NULL) {
					if (temp->ino == inode) {
						repeat = 1;
					}
					temp = temp->next;
				}
				if(!repeat) {
					temp->next = newNode;
				}
			}
			if (!repeat) {
				size += buf.st_size;
			}	
		} else {
			continue; // inacessible file
		}
	}
	if ((getenv("UNITS") != NULL) && strcasecmp(getenv("UNITS"), "k") == 0){
		printf("%dkb\n", size/1024);
	} else {
		printf("%d\n", size);
	}
	
	if (getenv("TMOM") != NULL) {
		//printf("set TMOM\n");
		pid= strtol(getenv("TMOM"), &endptr, 10);
		if (*endptr != '\0'){
	    	pid = -1;
		}
		if (pid > 0) {
			kill(pid, SIGUSR1);
		}
	} 
	exit(0);
}

