////////////////////////////////////////////////////////////////////////////////
// Main File:        send_signal.c
// This File:        send_signal.c
// Other Files:      my_c_signal_handler.c, my_div0_handler.c 
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
//	04/30	Started doing send_signal.c
//	04/30	Handling command line argument 
//	04/30	Hnadling sending 2 signals 
//	04/30	Finish the whole file
// 
///////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
// Date:   Name (email):   Helped with: (brief description)
// ---------------------------------------------------------------------------
// 
//////////////////////////// 80 columns wide ///////////////////////////////////

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<ctype.h>
#include<signal.h>

// check if string s is a number
int isNumber(char *s) {
	char current_char;
	int i = 0;
	
	// loop through every character of the string
	while(*(s+i) != '\0') {

		// get the current character
		current_char = *(s + i);

		// check if it is a digit
		if(!isdigit(current_char)) {
			return 0;
		}

		// get the next character
		++i;
	}

	return 1;
}

// the main method of the whole program
int main(int argc, char **argv) {
	// check the number of arguments
	if(argc != 3) {
		printf("Usage: send_signal <signal type> <pid>\n");
		exit(1);
	}
	
	// check if the first argument is in correct form
	if(strcmp(*(argv + 1), "-u") != 0 && strcmp(*(argv + 1), "-i") != 0) {
		printf("The <signal type> can only be -u or -i\n");
		exit(1);
	}

	// check if the last argument is a number
	if(!isNumber(*(argv + 2))) {
		printf("The <pid> can only be number\n");
		exit(1);
	}

	// get the pid number
	pid_t pid = atoi(*(argv + 2));
	
	// check if we are doing user signal
	if(strcmp(*(argv + 1), "-u") == 0) {
		// check if the signal sent is failed
		if(kill(pid, SIGUSR1) != 0) {
			printf("Error in sending SIGUSR1 to PID: %d\n", pid);
			exit(1);
		}
	} else{
		// interrupt case
		if(kill(pid, SIGINT) != 0) {
			printf("Error in sending SIGINT to PID: %d\n", pid);
			exit(1);
		}
	}

}
