# CS 415: Systems Programming 

## Project 1: Beetle

A C warm-up exercise. Simulates the "boiling" of a number of beetles on an X by X square, and returns the average mean lifetime before it falls off the edge.

## Project 2: RGPP

Formats grep output proved on STDIN. Line mode ("-l" flag) returns matching lines, where word mode ("-w" flag) counts the number of times a given word occurs in the input. "-b" invokes an optional banner and "-n" prints line numbers for line mode.

##Project 3: Forks and Pipes

Takes a list of file names on STDIN to the report executable. report forks and exec's the accessed and totalsize binaries, which returns the information back to report to aggregate and return information on STOUT.

##Project 4: Networking

Implements a game of nim using TCP sockets. Fire up nim_server and connect with two seperate nim clients to play. The server waits in the background until it receives two valid connections, then forks nim_match_server to host the game and then gracefully shuts down.

##Project 5: Shell Scripting

An scripting extenstion to project 1. Runs the beetle program after given a minimum square size, maximumu square size, and number of beetles. Can be invoked with additional command line arguments to plot the data to file with GNU plot. If hit with a SIGUSR2 mid-execution, the program will dump its current status into a statefile. The script can be run with the statefile fed on the commandline to resume at the same place execution was suspended.
