////////////////////////////////////////////////////////////////////////////////
// Main File:        my_magic_square.c 
// This File:        my_magic_square.c
// Other Files:      (name of all other files if any)
// Semester:         CS 354 Lecture 002      SPRING 2024
// Grade Group:      11  (See canvas.wisc.edu/groups for your gg#)
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
// 
// 
// 
// 
// 
///////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
// Date:   Name (email):   Helped with: (brief description)
// ---------------------------------------------------------------------------
// 
//////////////////////////// 80 columns wide ///////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission, CS 354 Spring 2024, Deb Deppeler
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure that represents a magic square
typedef struct {
	int size;           // dimension of the square
	int **magic_square; // pointer to heap allocated magic square
} MagicSquare;

/* TODO:
 * Promps the user for the magic square's size, read it,
 * check if it's an odd number >= 3 (if not, display the required
 * error message and exit)
 *
 * return the valid number
 */
int getSize() {
	printf("Enter magic square's size (odd integer >=3)\n");
	int size;
	scanf("%i", &size);
	
	// check if size is odd
	if(size % 2 == 0) {
		printf("Magic square size must be odd.\n");
		exit(1);
	}
	
	// check size >= 3
	if(size < 3) {
		printf("Magic square size must be >= 3.\n");
		exit(1);
	}

	return size;   
} 

/* TODO:
 * Makes a magic square of size n,
 * and stores it in a MagicSquare (on the heap)
 *
 * It may use the Siamese magic square algorithm 
 * or alternate from assignment 
 * or another valid algorithm that produces a magic square.
 *
 * n - the number of rows and columns
 *
 * returns a pointer to the completed MagicSquare struct.
 */
MagicSquare *generateMagicSquare(int n) {
	// create space for a magic square structure
	MagicSquare *magic_square = malloc(sizeof(MagicSquare));

	// check if the magic square is created successfully
	if(magic_square == NULL) {
		printf("Fail to create memory space for magic square structure on heap\n");
		exit(1);
	}

	// store the size of the magic square
	magic_square->size = n;
	
	// initialize the memory for the square
	magic_square->magic_square = malloc(sizeof(int *) * n);
	
	// check if the square is created successfully
	if(magic_square->magic_square == NULL) {
		printf("Fail to create memory space for magic square on heap\n");
		exit(1);
	}

	for(int i = 0; i < n; ++i) {
		*(magic_square->magic_square + i) = malloc(sizeof(int) * n);
		if(*(magic_square->magic_square + i) == NULL) {
			for(int j = 0; j < i; ++j) 
				free(*(magic_square->magic_square + j));
			free(magic_square->magic_square);
			printf("Fail to create memory space for magic square on heap\n");
			exit(1);
		}
	}

	// initialize the magic square to all 0
	for(int i = 0; i < n; ++i)
		for(int j = 0; j < n; ++j)
			*(*(magic_square->magic_square + i) + j) = 0;

	// keep track of the current coordinate
	int x = 0, y = n / 2;
	for(int num = 1; num <= n * n; ++num) {
		// store the value to the coordinate
		*(*(magic_square->magic_square + x) + y) = num;

		// find the new coordinate
		int new_x = (x - 1 + n) % n, new_y = (y + 1 + n) % n;
		if(*(*(magic_square->magic_square + new_x) + new_y) != 0) {
			new_x = x + 1;
			new_y = y;
		}

		// reassign the coordinate
		x = new_x;
		y = new_y;
	}	

	return magic_square;    
} 

/* TODO:  
 * Opens a new file (or overwrites the existing file)
 * and writes the magic square values to the file
 * in the specified format.
 *
 * magic_square - the magic square to write to a file
 * filename - the name of the output file
 */
void fileOutputMagicSquare(MagicSquare *magic_square, char *filename) {
	FILE *fp = fopen(filename, "w");
	fprintf(fp, "%i\n", magic_square->size);
	for(int i = 0; i < magic_square->size; ++i) {
		for(int j = 0; j < magic_square->size; ++j) {
			fprintf(fp, "%i", *(*(magic_square->magic_square + i) + j));
			if(j < magic_square->size - 1)
				fprintf(fp, ",");
			else
				fprintf(fp, "\n");
		}
	}
	fclose(fp);
}


/* TODO:
 * Generates a magic square of the user specified size and
 * outputs the square to the output filename.
 * 
 * Add description of required CLAs here
 */
int main(int argc, char **argv) {
	// TODO: Check input arguments to get output filename
	if(argc != 2) {
		printf("Usage: ./my_magic_square <output_filename>\n");
		exit(1);
	}
	// TODO: Get magic square's size from user
	int size = getSize();	
	
	// TODO: Generate the magic square by correctly interpreting 
	//       the algorithm(s) in the write-up or by writing or your own.  
	//       You must confirm that your program produces a 
	//       Magic Sqare as described in the linked Wikipedia page.
	MagicSquare *magic_square = generateMagicSquare(size);

	// TODO: Output the magic square
	fileOutputMagicSquare(magic_square, *(argv + 1));
	
	// free the magic square before the program ends
	for(int i = 0; i < magic_square->size; ++i)
		free(*(magic_square->magic_square + i));
	free(magic_square->magic_square);
	free(magic_square);
	return 0;
} 

// Spring 2024


