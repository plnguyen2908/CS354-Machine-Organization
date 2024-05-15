////////////////////////////////////////////////////////////////////////////////
// Main File:        N/A
// This File:		 p3Heap.c
// Other Files:      p3Heap.h, Makefile
// Semester:         CS 354 Lecture 2      SPRING 2024
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
//  02/28: Started p3A, read init_heap, disp_heap
//  02/29: Started implementing balloc 
//  02/29: Finished implementing balloc and passed partA test
//  02/29: Started implementing bfree 
//  02/29: Finished implementing bfree and passed partA, B, C, and D test 
///////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
// Date:   Name (email):   Helped with: (brief description)
// ---------------------------------------------------------------------------
// 
//////////////////////////// 80 columns wide ///////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2024 Deb Deppeler based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes.
// Used by permission SPRING 2024, CS354-deppeler
//
/////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include "p3Heap.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block.
 */
typedef struct blockHeader {           

    /*
     * 1) The size of each heap block must be a multiple of 8
     * 2) heap blocks have blockHeaders that contain size and status bits
     * 3) free heap block contain a footer, but we can use the blockHeader 
     *.
     * All heap blocks have a blockHeader with size and status
     * Free heap blocks have a blockHeader as its footer with size only
     *
     * Status is stored using the two least significant bits.
     *   Bit0 => least significant bit, last bit
     *   Bit0 == 0 => free block
     *   Bit0 == 1 => allocated block
     *
     *   Bit1 => second last bit 
     *   Bit1 == 0 => previous block is free
     *   Bit1 == 1 => previous block is allocated
     * 
     * Start Heap: 
     *  The blockHeader for the first block of the heap is after skip 4 bytes.
     *  This ensures alignment requirements can be met.
     * 
     * End Mark: 
     *  The end of the available memory is indicated using a size_status of 1.
     * 
     * Examples:
     * 
     * 1. Allocated block of size 24 bytes:
     *    Allocated Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 25
     *      If the previous block is allocated p-bit=1 size_status would be 27
     * 
     * 2. Free block of size 24 bytes:
     *    Free Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 24
     *      If the previous block is allocated p-bit=1 size_status would be 26
     *    Free Block Footer:
     *      size_status should be 24
     */
    int size_status;

} blockHeader;         

/* Global variable - DO NOT CHANGE NAME or TYPE. 
 * It must point to the first block in the heap and is set by init_heap()
 * i.e., the block at the lowest address.
 */
blockHeader *heap_start = NULL;     

/* Size of heap allocation padded to round to nearest page size.
 */
int alloc_size;

/*
 * Additional global variables may be added as needed below
 * TODO: add global variables needed by your function
 */




/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block (payload) on success.
 * Returns NULL on failure.
 *
 * This function must:
 * - Check size - Return NULL if size < 1 
 * - Determine block size rounding up to a multiple of 8 
 *   and possibly adding padding as a result.
 *
 * - Use BEST-FIT PLACEMENT POLICY to chose a free block
 *
 * - If the BEST-FIT block that is found is exact size match
 *   - 1. Update all heap blocks as needed for any affected blocks
 *   - 2. Return the address of the allocated block payload
 *
 * - If the BEST-FIT block that is found is large enough to split 
 *   - 1. SPLIT the free block into two valid heap blocks:
 *         1. an allocated block
 *         2. a free block
 *         NOTE: both blocks must meet heap block requirements 
 *       - Update all heap block header(s) and footer(s) 
 *              as needed for any affected blocks.
 *   - 2. Return the address of the allocated block payload
 *
 *   Return if NULL unable to find and allocate block for required size
 *
 * Note: payload address that is returned is NOT the address of the
 *       block header.  It is the address of the start of the 
 *       available memory for the requesterr.
 *
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* balloc(int size) {     
	// check if requested size < 1
	if(size < 1)
		return NULL;

	// add header to block_size
	int block_size = size + sizeof(blockHeader);

	// add padding to block_size if needed
	if(block_size % 8 != 0) 
		block_size += 8 - (block_size % 8);

	// get ptr and size of the best-fit block
	int best_size = alloc_size;
	blockHeader *best_ptr = NULL;	
	blockHeader *current = heap_start;

	while(current->size_status != 1) {
		// get current block size
		int current_size = (current->size_status - (current->size_status % 8));
		int a_bit = (current->size_status & 1);

		// only consider unallocated block
		if(a_bit == 0) {
			
			// cehck if the current block is closest in size
			if(best_size >= current_size && current_size >= block_size) {
				best_size = current_size;
				best_ptr = current;	
			}

			// check early stop
			if(best_size == block_size)
				break;
		}
		
		// get to the next block
		current = (blockHeader *)((void *)current + current_size);
		
	}

	// check best-fit block
	if(best_ptr != NULL){
		if(best_size > block_size) {
			// get the size after splitting
			int remaining_size = best_size - block_size;

			// get the next block
			blockHeader *next_block = (blockHeader *)((void *)best_ptr + block_size);
			
			// update the size and p-bit of the next block
			next_block->size_status = remaining_size  + 2;

			// get to the footer
			next_block = (blockHeader *)((void *)next_block + remaining_size - sizeof(blockHeader));

			// update the footer
			next_block->size_status = remaining_size;
		}
	
		// get the p_bit of the current block ptr
		int p_bit = best_ptr->size_status  & 2;
		
		// update the current block to the desired block
		best_ptr->size_status = block_size + p_bit + 1;

		// return the starting address of payload
		return ((void *)best_ptr + sizeof(blockHeader));
	}
	
	// alloc fails
    return NULL;
}

/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - Update header(s) and footer as needed.
 *
 * If free results in two or more adjacent free blocks,
 * they will be immediately coalesced into one larger free block.
 * so free blocks require a footer (blockHeader works) to store the size
 *
 * TIP: work on getting immediate coalescing to work after your code 
 *      can pass the tests in partA and partB of tests/ directory.
 *      Submit code that passes partA and partB to Canvas before continuing.
 */                    
int bfree(void *ptr) {    
	// check if ptr is null
	if(ptr == NULL) {
		return -1;
	} 

	// check if address of payload is multiple of 8
	if((unsigned long int)ptr % 8 != 0) {
		return -1;
	}

	// check if ptr is below starting point or above ending point
	if(((unsigned long int)ptr < (unsigned long int)heap_start) || ((unsigned long int)ptr > (unsigned long int)((void *)heap_start + alloc_size - 1))) {
		return -1;
	}

	// get info of the current block
	blockHeader *current_block = ptr - sizeof(blockHeader);
	int current_size = current_block->size_status - (current_block->size_status % 8);
	int a_bit = current_block->size_status & 1;
	int p_bit = (current_block->size_status >> 1) & 1;

	// check if block is already freed
	if(a_bit == 0){
		return -1;
	}

	// free the current block
	current_block->size_status -= 1;
	
	// create footer for the current block
	blockHeader *footer = (void *)current_block + current_size - sizeof(blockHeader);
	footer->size_status = current_size;
	
	// check the next block
	blockHeader *next_block = (void *)current_block + current_size;	 
	
	// check if the next block is not the end mark
	if(next_block->size_status != 1) {
		
		// check if the next block is allocated
		if(next_block->size_status & 1) {
			// set the p-bit of the next block to 0
			next_block->size_status -= 2;
		} else {
			// get the size of the next block
			int next_size = next_block->size_status - (next_block->size_status % 8);
			
			// coalesce next block with current block
			current_block->size_status += next_size;
			current_size += next_size;
			
			// create footer after merging
			footer = (void *)current_block + current_size - sizeof(blockHeader);
			footer->size_status = current_size;
		}
	}

	// check if the previous block is free
	if(p_bit == 0) {
		// get to the footer of the previous block
		footer = (void *)current_block - sizeof(blockHeader);
		
		// get the previous block's size
		int prev_size = footer->size_status;

		// get the previous block
		blockHeader *prev_block = (void *)current_block - prev_size;

		// coalesce the previous block with the current block
		prev_block->size_status += current_size;
		prev_size += current_size;

		// create footer after merging
		footer = (void *)prev_block + prev_size - sizeof(blockHeader);
		footer->size_status = prev_size;
	}

	return 0;
} 


/* 
 * Initializes the memory allocator.
 * Called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int init_heap(int sizeOfRegion) {    

	static int allocated_once = 0; //prevent multiple myInit calls

	int   pagesize; // page size
	int   padsize;  // size of padding when heap size not a multiple of page size
	void* mmap_ptr; // pointer to memory mapped area
	int   fd;

	blockHeader* end_mark;

	if (0 != allocated_once) {
		fprintf(stderr, 
				"Error:mem.c: InitHeap has allocated space during a previous call\n");
		return -1;
	}

	if (sizeOfRegion <= 0) {
		fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
		return -1;
	}

	// Get the pagesize from O.S. 
	pagesize = getpagesize();

	// Calculate padsize as the padding required to round up sizeOfRegion 
	// to a multiple of pagesize
	padsize = sizeOfRegion % pagesize;
	padsize = (pagesize - padsize) % pagesize;

	alloc_size = sizeOfRegion + padsize;

	// Using mmap to allocate memory
	fd = open("/dev/zero", O_RDWR);
	if (-1 == fd) {
		fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
		return -1;
	}
	mmap_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (MAP_FAILED == mmap_ptr) {
		fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
		allocated_once = 0;
		return -1;
	}

	allocated_once = 1;

	// for double word alignment and end mark
	alloc_size -= 8;

	// Initially there is only one big free block in the heap.
	// Skip first 4 bytes for double word alignment requirement.
	heap_start = (blockHeader*) mmap_ptr + 1;

	// Set the end mark
	end_mark = (blockHeader*)((void*)heap_start + alloc_size);
	end_mark->size_status = 1;

	// Set size in header
	heap_start->size_status = alloc_size;

	// Set p-bit as allocated in header
	// note a-bit left at 0 for free
	heap_start->size_status += 2;

	// Set the footer
	blockHeader *footer = (blockHeader*) ((void*)heap_start + alloc_size - 4);
	footer->size_status = alloc_size;

	return 0;
} 

/* STUDENTS MAY EDIT THIS FUNCTION, but do not change function header.
 * TIP: review this implementation to see one way to traverse through
 *      the blocks in the heap.
 *
 * Can be used for DEBUGGING to help you visualize your heap structure.
 * It traverses heap blocks and prints info about each block found.
 * 
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void disp_heap() {     

	int    counter;
	char   status[6];
	char   p_status[6];
	char * t_begin = NULL;
	char * t_end   = NULL;
	int    t_size;

	blockHeader *current = heap_start;
	counter = 1;

	int used_size =  0;
	int free_size =  0;
	int is_used   = -1;

	fprintf(stdout, 
			"********************************** HEAP: Block List ****************************\n");
	fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
	fprintf(stdout, 
			"--------------------------------------------------------------------------------\n");

	while (current->size_status != 1) {
		t_begin = (char*)current;
		t_size = current->size_status;

		if (t_size & 1) {
			// LSB = 1 => used block
			strcpy(status, "alloc");
			is_used = 1;
			t_size = t_size - 1;
		} else {
			strcpy(status, "FREE ");
			is_used = 0;
		}

		if (t_size & 2) {
			strcpy(p_status, "alloc");
			t_size = t_size - 2;
		} else {
			strcpy(p_status, "FREE ");
		}

		if (is_used) 
			used_size += t_size;
		else 
			free_size += t_size;

		t_end = t_begin + t_size - 1;

		fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%4i\n", counter, status, 
				p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

		current = (blockHeader*)((char*)current + t_size);
		counter = counter + 1;
	}

	fprintf(stdout, 
			"--------------------------------------------------------------------------------\n");
	fprintf(stdout, 
			"********************************************************************************\n");
	fprintf(stdout, "Total used size = %4d\n", used_size);
	fprintf(stdout, "Total free size = %4d\n", free_size);
	fprintf(stdout, "Total size      = %4d\n", used_size + free_size);
	fprintf(stdout, 
			"********************************************************************************\n");
	fflush(stdout);

	return;  
} 


//		p3Heap.c (SP24)                     

