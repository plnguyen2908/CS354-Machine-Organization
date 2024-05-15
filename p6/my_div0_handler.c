////////////////////////////////////////////////////////////////////////////////
// Main File:        my_div0_handler.c 
// This File:        my_div0_handler.c
// Other Files:      send_signal.c, my_c_signal_handler.c
// Semester:         CS 354 Lecture 002      SPRING 2024
// Grade Group:      gg11  (See canvas.wisc.edu/groups for your gg#)
// Instructor:       deppeler
// 
// Author:           Le Thien Phuc Nguyen
// Email:            plnguyen6@wisc.edu
// CS Login:         leos
//
/////////////////////////// SEARCH LOG //////////////////////////////// 
// Online sources: do not rely web searches to solve your problems, 
// but if you do search for help on a topic, include Query and URLs here.
// IF YOU FIND CODED SOLUTIONS, IT IS ACADEMIC MISCONDUCT TO USE THEM
//                               (even for "just for reference")
// Date:   Query:                      URL:
// --------------------------------------------------------------------- 
// 
// 
// 
// 
// 
// AI chats: save a transcript.  No need to submit, just have available 
// if asked.
/////////////////////////// COMMIT LOG  ////////////////////////////// 
//  Date and label your work sessions, include date and brief description
//  Date:   Commit Message: 
//  -------------------------------------------------------------------
//	04/30	Started doing the task: working on main function
//	04/30	Finish while loop in the main method 
//	04/30	Begin handling division by 0
//	04/30	Finish handling division by 0
//	04/30	Begin handling interrupt signal 
//	04/30	Finish the whole file 
///////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
// Date:   Name (email):   Helped with: (brief description)
// ---------------------------------------------------------------------------
// 
//////////////////////////// 80 columns wide ///////////////////////////////////

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<signal.h>

int cnt = 0;

// division by 0 handler
void SIGFPE_handler() {
	// print the error message
	printf("Error: a division by 0 operation was attempted.\n");

	// print the counter
	printf("Total number of operations completed successfully: %d\n", cnt);
	
	// printe the exiting message
	printf("The program will be terminated.\n");

	// exit the program
	exit(0);
}

// SIGINT handler
void SIGINT_handler() {
	// print the counter
	printf("\nTotal number of operations completed successfully: %d\n", cnt);
	
	// printe the exiting message
	printf("The program will be terminated.\n");

	// exit the program
	exit(0);
}

// main function of the program
int main(int argc, char **argv) {
	// create a sigaction for SIGFPE
	struct sigaction fpe_sa;

	// reset the sigaction struct
	memset(&fpe_sa, 0, sizeof(fpe_sa));

	// setup the handler for SIGFPE
	fpe_sa.sa_handler = SIGFPE_handler;

	// binding to the corresponding signal: SIGFPE
	if(sigaction(SIGFPE, &fpe_sa, NULL) != 0) {
		printf("Error binding SIGFPE handler\n");
		exit(1);
	}

	// create a sigaction for SIGINT
	struct sigaction int_sa;
	
	// reset the sigaction struct
	memset(&int_sa, 0, sizeof(int_sa));

	// setup the handler for SIGINT
	int_sa.sa_handler = SIGINT_handler;

	// binding to the corresponding signal: SIGINT
	if(sigaction(SIGINT, &int_sa, NULL) != 0) {
		printf("Error binding SIGINT handler\n");
		exit(1);
	}

	// loop infinitely
	while(1) {
		char input[100];

		// get the first integer
		printf("Enter first integer: ");
		if(fgets(input, 100, stdin) == NULL) {
			printf("Error reading the first number\n");
			exit(1);
		}
		int first_int = atoi(input);

		// get the second integer
		printf("Enter second integer: ");
		if(fgets(input, 100, stdin) == NULL) {
			printf("Error reading the second number\n");
			exit(1);
		}
		int second_int = atoi(input);
		
		// print the message
		printf("%d / %d is %d with a remainder of %d\n", first_int, second_int, first_int / second_int, first_int % second_int);
		
		// increase the counter if the operation is successful
		++cnt;
	}
}
