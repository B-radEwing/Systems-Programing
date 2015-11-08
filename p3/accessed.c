#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

typedef struct Node Node;

struct Node {
    long ino;
    Node *next;
};

void printUsage();

int main(int argc, char *argv[]) {
	int num;
	int elapsedTime;
	time_t theTime;
	long inode;
	struct stat buf;
	char files[4096];
	char *endptr;
	int repeat = 0;
	Node *head = NULL;
	Node *nextNode;
	Node *temp;

	/* Argument Checking */

	if (argc != 2) {
		printUsage();
	}

	num = strtol(argv[1], &endptr, 10);
	if (*endptr != '\0') {
	    fprintf(stderr, "error reading number on stdin\n");
	    printUsage();
	}

	if (num == 0) {
		fprintf(stderr, "error: 0 is an invalid number\n");
		printUsage();
	}

	//printf("%d\n", num);

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

                time(&theTime);
                elapsedTime = (theTime - buf.st_atime)/86400;
				//printf("time: %d\n", theTime);
				//printf("time: %d\n", buf.st_atime);
                if (num < 0) { //negative value
                    if (elapsedTime < abs(num)) {
                        printf("%s\n", files);
                    }
                } else {
                    if (elapsedTime >= num) {
                        printf("%s\n", files);
                    }
                        }
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
					
					time(&theTime);
					elapsedTime = (theTime - buf.st_atime)/86400;

					if (num < 0) {//negative value
						
						if (elapsedTime < abs(num)) {
							printf("%s\n", files);
						}
					} else { // positive value
						if (elapsedTime >= num) {
							printf("%s\n", files);
						}
					}
				}
			}
		} else {
			continue; // inacessible file;
		}
	}
	//printf(""); // output incase of no output, to prevent totalsize infite loop on scanf
	exit(0);

}

void printUsage() {
	fprintf(stderr, "usage: accessed [num]\n");
	printf("a positive int outputs regular files which have not been accessed for num days\n");
	printf("a negative int outputs regular files which have been accessed within num days\n");
	exit(1);
}
