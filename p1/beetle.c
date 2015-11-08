#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

int beetles;
int sideLength;
float meanLifetime;

int main (int argc, char *argv[]) {

	// Check for arguements
	if (argc != 3) {
		fprintf(stderr, "Error! Usage: ./beetle 6 10000\n");
		exit(1);
	}
	char *endFirst;
	errno = 0;
	int first_arg = strtol(argv[1], &endFirst, 10);
	if (errno == ERANGE) {
		if(first_arg == LONG_MIN) {
			fprintf(stderr, "Error! %d underflowed!\n", first_arg);
                        exit(1);
		} else if(first_arg == LONG_MAX) {
			fprintf(stderr, "Error! %d overflowed!\n", first_arg);
                        exit(1);
		}
	} else if (*endFirst != '\0') {
                fprintf(stderr, "First argument not a positive integer\n");
		exit(1);
	} else if (first_arg < 0) {
		fprintf(stderr, "First argument not a positive integer\n");
                exit(1);
        } else {
                sideLength = first_arg;
        }

	char *endSecond;
        errno = 0;
        int second_arg = strtol(argv[2], &endSecond, 10);
        if (errno == ERANGE) {
                if(second_arg == LONG_MIN) {
                        fprintf(stderr, "Error! %d underflowed!\n", second_arg);
                        exit(1);
                } else if(second_arg == LONG_MAX) {
                        fprintf(stderr, "Error! %d overflowed!\n", second_arg);
                        exit(1);
                }
        } else if (*endSecond != '\0') {
                fprintf(stderr, "Second argument not a positive integer\n");
                exit(1);
	} else if (second_arg < 0 ) {
		fprintf(stderr, "Second argument not a positive integer\n");
                exit(1);
        } else {
                beetles = second_arg;
        }	
	int run = 0;
	int moves = 0;
	int negLimit = (0 - sideLength);
	float X = 0;
	float Y = 0; 
	float conversion = (3.14159265358979323846 / 180);
	int temp;
	while (run < beetles) {
		while ((X < sideLength) && (X > negLimit) && ( Y < sideLength) && ( Y > negLimit)) {
			moves++;
			temp = (random() % 360);
			X += cos(conversion * temp);
			Y += sin(conversion * temp);
		}
		meanLifetime += moves;
		run++;
		moves = 0;
		X = 0;
		Y = 0;
	}
	meanLifetime = meanLifetime / beetles / 2;
	meanLifetime = meanLifetime * 10;
	meanLifetime = meanLifetime / 10.0;
	printf("%d by %d square, %d beetles, mean beetle lifetime is %.1f\n", sideLength, sideLength, beetles, meanLifetime);
	exit(1);
}
