////////////////////////////////////////////////////////////////////////////////
// Main File:        my_c_signal_handler.c 
// This File:        my_c_signal_handler.c
// Other Files:      send_signal.c, my_div0_handler.c
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
//  04/29	Begin doing the main function in my_c_signal_handler.c
//  04/29	Finish main function 
//	04/30	begin doing the rest: alarm and alarm handler 
//	04/30	finsih alarm handler and it works
//	04/30	begin handling SIGUSR1
//	04/30	finish handling SIGUSR1
//	04/30	begin handling interrput signal
//	04/30	finish the whole file	
//
// 
///////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
// Date:   Name (email):   Helped with: (brief description)
// ---------------------------------------------------------------------------
// 
//////////////////////////// 80 columns wide //////////////////////////////////

#include<stdio.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<signal.h>
#include<stdlib.h>
#include<sys/types.h>

// create a global variable to keep track of seconds of alarm
int seconds = 5;

// alarm handler
void alarm_handler() {
	// get the pid of the process
	pid_t pid = getpid();
	
	// get the time
	time_t t = time(NULL);

	// check if time returns correctly
	if(t == -1) {
		printf("time is not fetched correcly\n");
		exit(1);
	}
	
	char *t_string = ctime(&t);
	if(t_string == NULL) {
		printf("cannot convert time to string\n");
		exit(1);
	}

	// print the alarm information
	printf("PID: %d CURRENT TIME: %s", pid, t_string);

	// runs alarm again
	alarm(seconds);
}

int SIGUSR1_count = 0;

void SIGUSR1_handler() {
	printf("SIGUSR1 handled and counted!\n");
	++SIGUSR1_count;
}

void SIGINT_handler() {
	printf("\nSIGINT handled.\n");
	printf("SIGUSR1 was handled %d times. Exiting now.\n", SIGUSR1_count);
	exit(0);
}

int main() {
	// print the instruction to user
	printf("PID and time print every 5 seconds.\n");

	// print the interrupt instruction
	printf("Type Ctrl-C to end the program.\n");	
	
	// declare sigaction to handle SIGALRM
	struct sigaction alarm_sa;
	
	// reset the sigaction
	memset(&alarm_sa, 0, sizeof(alarm_sa));
	
	// register for the SIGALRM handler
	alarm_sa.sa_handler = alarm_handler;

	// binding the handler to corresponding SIGALRM signal
	if(sigaction(SIGALRM, &alarm_sa, NULL) != 0) {
		printf("Error binding SIGALRM handler\n");
		exit(1);
	}

	// declare sigaction to handle SIGUSR1
	struct sigaction user_sa;
	
	// reset the sigaction
	memset(&user_sa, 0, sizeof(user_sa));

	// register for the SIGUSER1 handler
	user_sa.sa_handler = SIGUSR1_handler;

	// binding the handler to corresponding SIGUSR1 signal
	if(sigaction(SIGUSR1, &user_sa, NULL) != 0) {
		printf("Error binding SIGUSER1 handler\n");
		exit(1);
	}

	// declare sigaction to handle SIGINT
	struct sigaction int_sa;

	// reset the sigaction
	memset(&int_sa, 0, sizeof(int_sa));

	// register for the SIGINT handler
	int_sa.sa_handler = SIGINT_handler;

	// binding the handler to corresosponding SIGINT signal
	if(sigaction(SIGINT, &int_sa, NULL) != 0) {
		printf("Error binding SIGINT handler\n");
		exit(1);
	}

	// runs alarm
	alarm(seconds);

	// runs an infinite loop
	while(1) {
	}
}
