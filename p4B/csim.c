////////////////////////////////////////////////////////////////////////////////
// Main File:        csim.c
// This File:        csim.c
// Other Files:		 N/A
// Semester:         CS 354 Lecture 2     SPRING 2024
// Grade Group:      gg11  (See canvas.wisc.edu/groups for your gg#)
// Instructor:       deppeler
// 
// Author:           Le Thien Phuc Nguyen
// Email:			 plnguyen6@wisc.edu
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
// 3/29/2024 Begin doing p4B
// 3/29/2024 Finish p4B
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

////////////////////////////////////////////////////////////////////////////////
// Copyright 2013,2019-2024
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission for Spring 2024
////////////////////////////////////////////////////////////////////////////////

/**
 * csim.c:  
 * A cache simulator that can replay traces (from Valgrind) and output
 * statistics to determine the number of hits, misses, and evictions.
 * The replacement policy is LRU (least-recently used).
 *
 * Implementation and assumptions:
 *  1. (L) load or (S) store cause at most one cache miss and a possible eviction.
 *  2. (I) Instruction loads are ignored.
 *  3. (M) Data modify is treated as a load followed by a store to the same
 *  address. Hence, an (M) operation can result in two cache hits, 
 *  or a miss and a hit plus a possible eviction.
 */  

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/******************************************************************************/
/* DO NOT MODIFY THESE VARIABLE NAMES and TYPES                               */
/* DO UPDATE THEIR VALUES AS NEEDED BY YOUR CSIM                              */

//Globals set by command line args.
int b = 0; //number of (b) bits
int s = 0; //number of (s) bits
int E = 0; //number of lines per set

//Globals derived from command line args.
int B; //block size in bytes: B = 2^b
int S; //number of sets: S = 2^s

//Global counters to track cache statistics in access_data().
int hit_cnt = 0;
int miss_cnt = 0;
int evict_cnt = 0;

//Global to control trace output
int verbosity = 0; //print trace if set
/******************************************************************************/


//Type mem_addr_t: Use when dealing with addresses or address masks.
typedef unsigned long long int mem_addr_t;

//Type cache_line_t: Use when dealing with cache lines.
//TODO - COMPLETE THIS TYPE
typedef struct cache_line {                    
    char valid;
    mem_addr_t tag;
    //Add a data member as needed by your implementation
	int used;
} cache_line_t;

//Type cache_set_t: Use when dealing with cache sets
//Note: Each set is a pointer to a heap array of one or more cache lines.
typedef cache_line_t* cache_set_t;

//Type cache_t: Use when dealing with the cache.
//Note: A cache is a pointer to a heap array of one or more sets.
typedef cache_set_t* cache_t;

// Create the cache we're simulating. 
//Note: A cache is a pointer to a heap array of one or more cache sets.
cache_t cache;  

/* TODO - COMPLETE THIS FUNCTION
 * init_cache:
 * Allocates the data structure for a cache with S sets and E lines per set.
 * Initializes all valid bits and tags with 0s.
 */                    
void init_cache() {
	B = (1<<b); // calculate number of bytes in a block
	S = (1<<s); // calculate number of sets in a cache

	// initialize the cachhe
	cache = malloc(S * sizeof(cache_set_t));
	if(cache == NULL) {
		printf("fail to initialize cache");
		exit(1);
	}

	// loop through each set in a cache
	for(int i = 0; i < S; ++i) {

		// initialize each cache's set
		*(cache + i) = malloc(E * sizeof(cache_line_t));
		if((cache + i) == NULL) {
			printf("fail to initialize set in cache");
		}

		// initialize each line in a set
		for(int j = 0; j < E; ++j) {
			(*(cache + i) + j)->valid = '0';
			(*(cache + i) + j)->tag = 0;
			(*(cache + i) + j)->used = 0;
		}
	}          
}


/* TODO - COMPLETE THIS FUNCTION 
 * free_cache:
 * Frees all heap allocated memory used by the cache.
 */                    
void free_cache() {            
	// loop through each set
	for(int i = 0; i < S; ++i) {
		// free the set
		free(*(cache + i));
	} 

	// free the whole cache
	free(cache);
}


/* TODO - COMPLETE THIS FUNCTION 
 * access_data:
 * Simulates data access at given "addr" memory address in the cache.
 *
 * If already in cache, increment hit_cnt
 * If not in cache, cache it (set tag), increment miss_cnt
 * If a line is evicted, increment evict_cnt
 */                    
void access_data(mem_addr_t addr) {     
	// get the id of the set
	int set_id = (addr >> b) % S;

	// get the set with lines
	cache_set_t set = *(cache + set_id);

	// get t-bits of the address
	mem_addr_t t_bits = (addr >> (b + s));
	
	// find the most recently used line counter
	int max_counter = 0;
	for(int i = 0; i < E; ++i) {
		cache_line_t *line = (set + i);
		if(line->valid == '1' && max_counter < line->used)
			max_counter = line->used;
	}

	// check if any valid line has the same tag as t_bits
	for(int i = 0; i < E; ++i) {
		cache_line_t *line = (set + i);
		if(line->valid == '1' && line->tag == t_bits) {
			
			// increment hit count if found
			hit_cnt++; 

			// update the counter
			line->used = max_counter + 1;

			// if in verbose mode, print hit
			if(verbosity)	
				printf("hit ");

			// exit if hit
			return;
		}
	}

	// miss case
	miss_cnt++;

	// if in verbose mode, print miss
	if(verbosity)
		printf("miss ");


	// find if there is a empty spot to insert new addr
	for(int i = 0; i < E; ++i) {
		cache_line_t *line = (set + i);
		if(line->valid == '0') {
			// fill the addr into the line
			line->tag = t_bits;
			line->valid = '1';
			line->used = max_counter + 1;

			// after fill the addr in, exit
			return;
		}
	}

	// if not, we have to evict
	evict_cnt++;
	
	// if in verbosity mode, print eviction
	if(verbosity)
		printf("eviction ");

	// find the least recently used
	int LRU = INT_MAX;
	cache_line_t *evicted_line = NULL;

	// loop through each line
	for(int i = 0; i < E; ++i) {
		cache_line_t *line = (set + i);

		// if the current line is used least recently, store it
		if(LRU > line->used) {
			LRU = line->used;
			evicted_line = line;
		}
	}
	//printf("%llx ", evicted_line->tag);
	// replace the evicted line
	evicted_line->tag = t_bits;
	evicted_line->used = max_counter + 1;
}


/* TODO - FILL IN THE MISSING CODE
 * replay_trace:
 * Replays the given trace file against the cache.
 *
 * Reads the input trace file line by line.
 * Extracts the type of each memory access : L/S/M
 * TRANSLATE each "L" as a load i.e. 1 memory access
 * TRANSLATE each "S" as a store i.e. 1 memory access
 * TRANSLATE each "M" as a load followed by a store i.e. 2 memory accesses 
 */                    
void replay_trace(char* trace_fn) {           
    char buf[1000];  
    mem_addr_t addr = 0;
    unsigned int len = 0;
    FILE* trace_fp = fopen(trace_fn, "r"); 

    if (!trace_fp) { 
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);   
    }

    while (fgets(buf, 1000, trace_fp) != NULL) {
        if (buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);

            if (verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

            // TODO - MISSING CODE
            // GIVEN: 1. addr has the address to be accessed
            //        2. buf[1] has type of acccess(S/L/M)
            // call access_data function here depending on type of access
			if (buf[1] == 'L'|| buf[1] == 'S')
				access_data(addr);
			else if(buf[1] == 'M') {
				access_data(addr);
				access_data(addr);
			}

            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}  


/*
 * print_usage:
 * Print information on how to use csim to standard output.
 */                    
void print_usage(char* argv[]) {                 
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Verbose flag.\n");
    printf("  -s <num>   Number of s bits for set index.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of b bits for word and byte offsets.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}  


/*
 * print_summary:
 * Prints a summary of the cache simulation statistics to a file.
 */                    
void print_summary(int hits, int misses, int evictions) {                
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}  


/*
 * main:
 * parses command line args, 
 * makes the cache, 
 * replays the memory accesses, 
 * frees the cache and 
 * prints the summary statistics.  
 */                    
int main(int argc, char* argv[]) {                      
    char* trace_file = NULL;
    char c;

    // Parse the command line arguments: -h, -v, -s, -E, -b, -t 
    while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
        switch (c) {
            case 'b':
                b = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'h':
                print_usage(argv);
                exit(0);
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbosity = 1;
                break;
            default:
                print_usage(argv);
                exit(1);
        }
    }

    //Make sure that all required command line args were specified.
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        print_usage(argv);
        exit(1);
    }

    //Initialize cache.
    init_cache();

    //Replay the memory access trace.
    replay_trace(trace_file);

    //Free memory allocated for cache.
    free_cache();

    //Print the statistics to a file.
    //DO NOT REMOVE: This function must be called for test_csim to work.
    print_summary(hit_cnt, miss_cnt, evict_cnt);
    return 0;   
}  

// 202401                                     

