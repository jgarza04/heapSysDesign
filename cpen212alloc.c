#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "cpen212alloc.h"
#include "cpen212common.h"



void *cpen212_init(void *heap_start, void *heap_end) {
    if(heap_start == NULL){ return NULL; }
    if(heap_end == NULL) { return NULL; }
    // Make sure heap_start and heap_end are 8B aligned
    if ((uintptr_t)heap_start % 8 != 0){
	    printf("Modifing heap_start to be 8B align.");
        uintptr_t addr = (uintptr_t)heap_start;
        uintptr_t aligned_addr = (addr + 7) & ~((uintptr_t)7);
        heap_start = (void *)aligned_addr;
    }
    if ((uintptr_t)heap_end % 8 != 0) {
	    printf("Modifing heap_end to be 8B align.");
        uintptr_t addr = (uintptr_t)heap_end;
        uintptr_t aligned_addr = (addr + 7) & ~((uintptr_t)7);
        heap_end = (void *)aligned_addr;
    }
    // Make sure heap size fits the minimum block size
    if((uintptr_t)heap_start + sizeof(heap_header) + 32 >= (uintptr_t)heap_end){
        printf("heap size should be at least 32 bytes");
        return NULL;
    }
    

    // Create a heap header at the start of the heap
    heap_header* header = (heap_header*)heap_start;
    header->heap_start = heap_start;
    header->heap_end = heap_end;

    size_t heap_size = (uintptr_t)heap_end - (uintptr_t)heap_start - sizeof(heap_header);

    // Create a block to be the size of the rest of the heap and set it to be free
    block_header* firstBlockHeader = (block_header*)(((uintptr_t)header->heap_start + sizeof(heap_header)));
    firstBlockHeader->alloc = NULL;
    firstBlockHeader->alloc = false;
    firstBlockHeader->next = NULL;
    firstBlockHeader->storage = heap_size - sizeof(block_header);

    return (void *)firstBlockHeader;
}

void *cpen212_alloc(void *heap_handle, size_t nbytes) {
    if(heap_handle == NULL){ return NULL; }
    if(!nbytes){ return NULL; }
    if((uintptr_t)heap_handle % 8 != 0){
	    printf("Not aligned heap handle\n");
	    return NULL; 
    }
    if(nbytes == 0){ return NULL; }
    if (nbytes % 8 != 0) {
        size_t aligned_sz = (nbytes + 7) & ~((size_t)7);
        nbytes = aligned_sz;
    }
    heap_header* header = (heap_header*)((uintptr_t)heap_handle - sizeof(heap_header));
    if((uintptr_t)heap_handle != ((uintptr_t)header->heap_start + sizeof(heap_header))){
        printf("Invalid heap handle.\n");
        return NULL;
    }
    size_t heap_size = (uintptr_t)header->heap_end - (uintptr_t)header->heap_start - sizeof(heap_header);
    size_t blocksize = nbytes + sizeof(block_header);
    if(blocksize > heap_size){
        printf("Not enough space.\n");
        return NULL;
    }
    for(block_header* curr = (block_header*)heap_handle; curr != NULL; curr = curr->next){
        // Check if block is allocated
        if(!curr->alloc){
            // Check if the free block fits the data to be stored
            if(curr->storage >= nbytes){
                // Check if the free block would have enough space remaining to fit another block.
                // If block could fit another block, we will split into two blocks and allocate one.
                // If block could not fit another block, we will just allocate the whole block.
                if(curr->storage - nbytes >= 32){
                    // Create the new free block with the smaller size
                    block_header* nextBlock = (block_header*)((uintptr_t)curr + blocksize);
		    nextBlock->alloc = NULL;
		    nextBlock->alloc = false;
                    nextBlock->storage = curr->storage - blocksize;
                    nextBlock->next = curr->next;

                    // Change the previous blocks size and status
		    curr->alloc = NULL;
		    curr->alloc = true;
                    curr->next = nextBlock;
                    curr->storage = nbytes;
                    return (void *)((uintptr_t)curr + sizeof(block_header));
                }else{
                    // Allocate the whole block
		    curr->alloc = NULL;
			curr->alloc = true;
                    return (void *)((uintptr_t)curr + sizeof(block_header));
                }
            }
        }
    }
    return NULL;
}


void cpen212_free(void *s, void *p) {
    // Assert s and p to not be null
    assert(s);
    assert(p);
    // Assert that s and p are both 8B align and within the heap's frame
    if((uintptr_t)s % 8 != 0 || (uintptr_t)p % 8 != 0){
	    printf("Either p or s not 8B aligned\n");
	    return;
    }
    heap_header* header = (heap_header*)((uintptr_t)s - sizeof(heap_header));
    if((uintptr_t)s != ((uintptr_t)header->heap_start + sizeof(heap_header))){
        printf("Invalid heap handle.");
        return;
    }
    if((uintptr_t)p >= (uintptr_t)header->heap_end || (uintptr_t)p < (uintptr_t)s){
	    printf("p not in heap\n");
	    assert((uintptr_t)p < (uintptr_t)header->heap_end && (uintptr_t)p >= (uintptr_t)s);
    }
    block_header* p_header = (block_header*)((uintptr_t)p - sizeof(block_header));

    bool lastAlloc = NULL;
    size_t lastStorage = NULL;
    // Check that the pointer p is actually pointing to one of the data blocks.
    for(block_header* curr = (block_header*)s; curr != NULL; curr = curr->next){
        if(curr == p_header){
		p_header->alloc = NULL;
            p_header->alloc = false;
	    block_header* nextBlock = (block_header*)p_header->next;
            if(lastStorage != NULL && !lastAlloc && nextBlock != NULL && !nextBlock->alloc){
                block_header* prevBlock = (block_header*)((uintptr_t)p_header - lastStorage - sizeof(block_header));
                prevBlock->storage += (2 * sizeof(block_header)) + p_header->storage + nextBlock->storage;
                prevBlock->next = nextBlock->next;
		p_header->next = NULL;
		p_header->storage = (size_t)0;
		nextBlock->storage = (size_t)0;
		nextBlock->next = NULL;
            }else if(lastStorage != NULL && !lastAlloc){
                block_header* prevBlock = (block_header*)((uintptr_t)p_header - lastStorage - sizeof(block_header));
                prevBlock->storage += sizeof(block_header) + p_header->storage;
                prevBlock->next = p_header->next;
		p_header->next = NULL;
                p_header->storage = (size_t)0;
            }else if(p_header->next != NULL && !p_header->next->alloc){
                p_header->next = nextBlock->next;
                p_header->storage += nextBlock->storage + sizeof(block_header);
		nextBlock->storage = (size_t)0;
                nextBlock->next = NULL;
            }
            return;
        }
        lastAlloc = curr->alloc;
        lastStorage = curr->storage;
    }
}

void *cpen212_realloc(void *s, void *prev, size_t nbytes){
    assert(s);
    assert(prev);
    if(nbytes == 0){
        cpen212_free(s, prev);
        return NULL;
    }
    // Assert that s and p are both 8B align and within the heap's frame
    if((uintptr_t)s % 8 != 0 || (uintptr_t)prev % 8 != 0){
            printf("Either prev or s not 8B aligned\n");
	    return NULL;
    }

    // Assert that nbytes is 8B align
    if (nbytes % 8 != 0) {
        size_t aligned_sz = (nbytes + 7) & ~((size_t)7);
        nbytes = aligned_sz;
    }

    heap_header* header = (heap_header*)((uintptr_t)s - sizeof(heap_header));
    if((uintptr_t)s != ((uintptr_t)header->heap_start + sizeof(heap_header))){
        printf("Invalid heap handle.");
        return NULL;
    }
    if((uintptr_t)prev>= (uintptr_t)header->heap_end || (uintptr_t)prev < (uintptr_t)s){
            printf("prev not in heap\n");
	    assert((uintptr_t)prev < (uintptr_t)header->heap_end && (uintptr_t)prev >= (uintptr_t)s);
    }
    block_header* p_header = (block_header*)((uintptr_t)prev - sizeof(block_header));
    block_header* prevBlock = NULL;
    for(block_header* curr = (block_header*)s; curr != NULL; curr = curr->next){
	    if(curr == p_header){ break; }
	    prevBlock = curr;
	    if(curr->next == NULL){
		    printf("Not valid prev\n");
			    return NULL;
		    }
    }

    
    //Check if both before and after are free to extend to
    if(prevBlock != NULL && p_header->next != NULL && !prevBlock->alloc && !p_header->next->alloc && (prevBlock->storage +
			    p_header->storage + p_header->next->storage + 2*sizeof(block_header)) >= nbytes){ 
	    // Check if would leave space for an extra block
	    if(prevBlock->storage + p_header->storage + p_header->next->storage + 2*sizeof(block_header) - 32 >= nbytes){
	    	// Get previous block, allocate it
                prevBlock->alloc = true;

                // Store the p_header stats
                size_t pStorage = p_header->storage;
                block_header* pNext = p_header->next;
		// Store the next block stats
		size_t nStorage = p_header->next->storage;
		block_header* nNext = p_header->next->next;

                // Copy the memory over
                // Get the start of ehe old data and a appropriate pointer for new data
                char* oldData = (char *)prev;
                char* newDataPtr = (char *)((uintptr_t)prevBlock + sizeof(block_header));

                // Copy the memory
		if(pStorage > nbytes){
			for (size_t i = 0; i < nbytes; i++) {
                        	newDataPtr[i] = oldData[i];
                	}
		}else{
			for (size_t i = 0; i < pStorage; i++) {
                                newDataPtr[i] = oldData[i];
                        }
		}

                // Create the newblock that will be at the front
                block_header* newBlock = ((uintptr_t)prevBlock + nbytes + sizeof(block_header));
                newBlock->alloc = false;
                newBlock->storage = prevBlock->storage + pStorage + sizeof(block_header) + nStorage - nbytes;
                newBlock->next = nNext;

                // Set the next and storage of the previous block.
                prevBlock->next = newBlock;
                prevBlock->storage = nbytes;

                // Return pointer to data
                return (void*)(newDataPtr);
	    }
	    // Use both block's before and after completely
	    
	    // Get previous block, allocate it
            prevBlock->alloc = true;
	    prevBlock->storage = prevBlock->storage + p_header->storage + 2*sizeof(block_header) + p_header->next->next;
	    prevBlock->next = p_header->next->next;

	    size_t prStorage = p_header->storage;
	    // Copy the memory over
            // Get the start of ehe old data and a appropriate pointer for new data
            char* oldData = (char *)prev;
            char* newDataPtr = (char *)((uintptr_t)prevBlock + sizeof(block_header));

            // Copy the memory
	    for (size_t i = 0; i < prStorage; i++){
		    newDataPtr[i] = oldData[i];
	    }

	    // Return pointer to data
	    return (void*)newDataPtr;
    }




    // Check if the new space is smaller than the previous space
    // If so check if the remaining block would be large enough to store some data or could combine with the next one.
    if(p_header->storage >= nbytes){
	
	if(p_header->next != NULL && !p_header->next->alloc){ // First case: Shrink with space in front
		// Store next block stats
		block_header* nextBlock = (block_header*)p_header->next;
		size_t nextStorage = nextBlock->storage;
		block_header* nextNext = nextBlock->next;
		
		// Create the new Block after the stored bytes
		block_header* newBlock = (block_header*)((uintptr_t)p_header + nbytes + sizeof(block_header));
		
		// Clear the next block to avoid interferance.
		nextBlock->storage = (size_t)0;
		nextBlock->next = NULL;

		// Allocate the information for the new block
		newBlock->next = nextNext;
		newBlock->alloc = false;
		newBlock->storage = nextStorage + p_header->storage - nbytes;
	
		// Update p's status
		p_header->next = newBlock;
		p_header->storage = nbytes;

		// Return the original pointer
		return (void*)prev;	
	}
	if(prevBlock != NULL && !prevBlock->alloc){ // Second case: shrink with space in the back
		// Get previous block, allocate it
		prevBlock->alloc = true;

		// Store the p_header stats
		size_t pStorage = p_header->storage;
		block_header* pNext = p_header->next;

		// Copy the memory over
		// Get the start of ehe old data and a appropriate pointer for new data
    		char* oldData = (char *)prev;
    		char* newDataPtr = (char *)((uintptr_t)prevBlock + sizeof(block_header));

    		// Copy the memory
    		for (size_t i = 0; i < nbytes; i++) {
        		newDataPtr[i] = oldData[i];
    		}

		// Create the newblock that will be at the front
		block_header* newBlock = ((uintptr_t)prevBlock + nbytes + sizeof(block_header));
		newBlock->alloc = false;
		newBlock->storage = prevBlock->storage + pStorage - nbytes;
		newBlock->next = pNext;
		
		// Set the next and storage of the previous block.
		prevBlock->next = newBlock;
		prevBlock->storage = nbytes;
		return (void*)(newDataPtr);
	}
	if(p_header->storage - 32 >= nbytes){ // Third case part 1: shrinking in its own block with spare space
        	block_header* newBlock = (block_header*)((uintptr_t)p_header + sizeof(block_header) + nbytes);
	    	newBlock->alloc = false;
	    	newBlock->alloc = false;
            	newBlock->next = p_header->next;
            	p_header->next = newBlock;
            	newBlock->storage = p_header->storage - nbytes - sizeof(block_header);
            	p_header->storage = nbytes;
            	return (void*)prev;
        }
	return (void*)prev; // Third case part 2: shrinking in its own block without spare space (no change)
    }

    // Check if expanding to the next block is possible.
    if(p_header->next != NULL && !p_header->next->alloc && p_header->next->storage + p_header->storage + sizeof(block_header) >= nbytes){ // Fourth case: extend to the front 
	    // Check if after expanding to next block there is still space for another block
	    void * nextToNext = p_header->next->next;
            size_t  nextStorage = p_header->next->storage;
	    if(p_header->next->storage + p_header->storage + sizeof(block_header) - nbytes >= 32){ // 4.1 Expanding to front with spare block
		    p_header->next->next = NULL;
                    p_header->next->storage = (size_t)0;
		    block_header* newBlock = (block_header*)((uintptr_t)prev + nbytes);
		    newBlock->next = nextToNext;
		    newBlock->storage = p_header->storage + nextStorage + sizeof(block_header) - nbytes;
		    newBlock->alloc = NULL;
		    newBlock->alloc = false;
		    p_header->next = newBlock;
		    p_header->storage = nbytes;
	    }

	    // 4.2 Expanding to front with no space block
	    p_header->next->alloc = NULL;
	    p_header->next->alloc = false;
	    p_header->next->next = NULL;
	    p_header->next->storage = (size_t)0;
	    p_header->storage = nextStorage + p_header->storage + sizeof(block_header);
	    p_header->next = nextToNext;
	    return (void*)prev;
    }

    // Check if expanding to the previous block is possible
    // Fifth case: Expanding to previous block
    if(prevBlock != NULL && !prevBlock->alloc && (prevBlock->storage + p_header->storage + sizeof(block_header)) >= nbytes){
	   // Check if expanding to prev block would leave space for another block    
	   block_header* pNext = p_header->next;
	   size_t pStorage = p_header->storage;
	   if((prevBlock->storage + p_header->storage + sizeof(block_header) - 32) >= nbytes){ // 5.1 Expanding to back with spare block
		// Get previous block, allocate it
                prevBlock->alloc = true;

                // Store the p_header stats
                size_t pStorage = p_header->storage;
                block_header* pNext = p_header->next;

                // Copy the memory over
                // Get the start of ehe old data and a appropriate pointer for new data
                char* oldData = (char *)prev;
                char* newDataPtr = (char *)((uintptr_t)prevBlock + sizeof(block_header));

                // Copy the memory
                for (size_t i = 0; i < pStorage; i++) {
                        newDataPtr[i] = oldData[i];
                }

                // Create the newblock that will be at the front
                block_header* newBlock = ((uintptr_t)prevBlock + nbytes + sizeof(block_header));
                newBlock->alloc = false;
                newBlock->storage = prevBlock->storage + pStorage - nbytes;
                newBlock->next = pNext;

                // Set the next and storage of the previous block.
                prevBlock->next = newBlock;
                prevBlock->storage = nbytes;

		// Return pointer to data
                return (void*)(newDataPtr);
	   }
	   // 5.2 Expanding to the back without spare block (only one block)

	   // Get previous block, allocate it
	   prevBlock->alloc = true;

	   // Copy the memory over
	   // Get the start of ehe old data and a appropriate pointer for new data
           char* oldData = (char *)prev;
           char* newDataPtr = (char *)((uintptr_t)prevBlock + sizeof(block_header));

           // Copy the memory
           for (size_t i = 0; i < pStorage; i++) {
		   newDataPtr[i] = oldData[i];
           }
	   // Set the prevBlock storage and next
	   prevBlock->storage = pStorage + prevBlock->storage + sizeof(block_header);
	   prevBlock->next = pNext;

	   // Return pointer to data
	   return (void*)newDataPtr;
    }


    // Check if allocating is possible, if so, deallocate the prev block, if not possible return NULL.
    void* newData = cpen212_alloc(s, nbytes);
    if(newData == NULL){ return NULL; }
    cpen212_free(s, prev);

    // Get the start of the old data and a appropriate pointer for new data
    char* oldData = (char *)p_header + sizeof(block_header);
    char* newDataPtr = (char *)newData;
                
    // Copy the memory
    for (size_t i = 0; i < p_header->storage; i++) {
        newDataPtr[i] = oldData[i]; 
    }

    return (void*)newDataPtr;

}
