#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Node Node;
typedef struct List List;

struct Node {
    char *fileName;
    char *fileLine;
    Node *next;
};

struct List {
    int length;
    Node *head;
};

void printUsage();

int main(int argc, char *argv[]) {

  int wordMode = 0;
  int lineMode = 0;
  int bMode = 0;
  int nMode = 0;
  char *word; // wordline search term
  int wordCount = 0;
  char input[4096];
  char *tokens;
  char *tempLine;
  int size;
  char *previousFile = "";
  char *currentFile = "";
  char *endptr;
  char *tempPhrase;
  int lineCount = 0;
  int tagLine;
  int wordLength;
  int diff;
  int count;
  FILE *fp;

  /* Argument Checking */
  if (argc == 1) {
      printf("error: program requires arguments\n");
      printUsage();
  }

  if (argc > 5) {
    printf("error: too many arguments provided\n");
    printUsage();
  }

  int i; // walk through argv
  for (i = 1; i < argc; i++) {
    if (((strcmp(argv[i], "-l")) == 0) || ((strcmp(argv[i], "-w")) == 0) || ((strcmp(argv[i], "-n")) == 0) || ((strcmp(argv[i], "-b")) == 0) ) {
      if (i == 1) {
        if ((strcmp(argv[i], "-l")) == 0) {
          lineMode = 1;
        } else if ((strcmp(argv[i], "-w")) == 0) {
          wordMode = 1;
          i++;
          if (i == argc) {
            printf("error: must provide word after -w\n");
            printUsage();
          }
          word = argv[i];
	  wordLength = strlen(word);
        } else {
          printf("error: first argument provided must be either -l or -w\n");
          printUsage();
        }
    } else {
      if ((strcmp(argv[i], "-n")) == 0) {
        nMode = 1;
      } else if ((strcmp(argv[i], "-b")) == 0) {
        bMode = 1;
      }

      if (((strcmp(argv[i], "-l")) == 0) && (wordMode == 1)) {
        printf("error: cannot run both modes\n");
        printUsage();
      }
      if (((strcmp(argv[i], "-w")) == 0) && (lineMode == 1)) {
        printf("error: cannot run both modes\n");
        printUsage();
      }
    }
  } else {
    printf("error: invalid argument provided\n");
    printUsage();
  }

  }

  /* Parse stdin */
  // Create linked list
  List *list = malloc(sizeof(List));
  list->length = 0;
  list->head = NULL;

  while (fgets(input, sizeof(input), stdin) != NULL) {
	
    for(i = 0; input[i]; i++){
                input[i] = tolower(input[i]);
    }

    // count words
    if (wordMode) {
      tempLine = malloc(strlen(input)+1);
      strcpy(tempLine, input);

      //http://cboard.cprogramming.com/c-programming/73365-how-use-strstr-find-all-occurrences-substring-string-not-only-first.html
      while ((tempLine = strstr(tempLine, word)) != NULL ) {
        wordCount++;
        tempLine++;

      }
    }

    // new node
    Node *nextNode = malloc(sizeof(Node));
    nextNode->next = NULL;
    nextNode->fileName = "";
    nextNode->fileLine = "";

    // parse input
    tokens = strtok(input, ":");
    size = strlen(tokens);
    nextNode->fileName = malloc(size+1);
    strcpy(nextNode->fileName, tokens);
    tokens = strtok(NULL, ":");
    size = strlen(tokens);
    nextNode->fileLine = malloc(size+1);
    strcpy(nextNode->fileLine, tokens);

    /* Linked List Insertion
     * https://github.com/B-radEwing/thread_lab/blob/master/queue.c */
    if (list->head == NULL) { //first insertion
      list->head = nextNode;
    } else {
      Node *temp = list->head;
      while (temp->next != NULL) {
        temp = temp->next;
      }
      temp->next = nextNode;
    }
    list->length++;
  }

  /* Print Banner */
  if (lineMode && bMode) {
    printf("\n\nTHERE ARE %d LINES THAT MATCH\n\n", list->length);
  } else if (wordMode && bMode) {
    printf("\n\nTHERE ARE %d MATCHING WORDS\n\n", wordCount);
  }

  /* Output */
 
  if (wordMode && (wordCount == 0)) {
    exit(1);
  }

  const Node *n = list->head;

  while (n) {
    currentFile = n->fileName;
    if (strcmp(currentFile, previousFile) != 0) {
      if (strcmp(previousFile, "") != 0) {
        fclose(fp);
      }

      fp = fopen(currentFile, "r");
      if (fp == NULL) {
        fprintf(stderr, "error: couldn't open file %s\n", currentFile);
        exit(1);
      }
      printf("=====================%s\n", currentFile);

      lineCount = 1;
      while (fgets(input, sizeof(input), fp)) {
        tagLine = strtol(n->fileLine, &endptr, 10);
	if (*endptr != '\0') {
	    fprintf(stderr, "error reading file line number: %s\n", n->fileLine);
	    printUsage();
	}
	for(i = 0; input[i]; i++){
  		input[i] = tolower(input[i]);
	}

        if (tagLine == lineCount) {
          if (n->next != NULL) {
            n = n->next;
          }
        }
        if (lineMode) {
          if (tagLine == lineCount) {
            printf("*");
          } else {
            printf(" ");
          }
        }

        if (nMode) {
          printf("%d:    ", lineCount);
        }

        if (wordMode) {
          if (tagLine == lineCount) {
            tempLine = malloc(strlen(input)+1);
            tempPhrase = malloc(strlen(input)+1);
            strcpy(tempLine, input);
            strcpy(tempPhrase, input);
            while ((tempLine = strstr(tempLine, word)) != NULL ) {
              count = 0;
              diff = strlen(tempPhrase) - strlen(tempLine);
              while (count < diff) {
                printf("%c", tempPhrase[count]);
                count++;
              }
              printf("\e[7m%s\e[0m", word);
              tempLine += wordLength;
              strcpy(tempPhrase, tempLine);

            }
            printf("%s", tempPhrase); // print whatever is left

          } else {
            printf("%s", input);
          }
        }
        if (lineMode) {
          printf("%s", input);
        }
        lineCount += 1;
      }
    }

    if(n->next == NULL) {
      fclose(fp);
      exit(1);
    }

    previousFile = currentFile;
  }

  exit(0);
}

void printUsage() {
  fprintf(stderr, "usage: rgpp [-l | -w word] {-n} {-b}\n");
  exit(1);
}
