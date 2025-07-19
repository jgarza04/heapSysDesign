#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "cpen212alloc.h"
#include "cpen212common.h"

#define MY_MEM_SIZE 352
char mymemory[MY_MEM_SIZE];

#define TOO_SMALL 24
char mymemory2[24];

int main()
{
    void * heap_handle;
    
    heap_handle = cpen212_init(&mymemory2[0], mymemory2 + TOO_SMALL);
    assert(heap_handle == NULL);

    heap_handle = cpen212_init(&mymemory[0], mymemory + MY_MEM_SIZE);
    assert(heap_handle != NULL);

    printf("\n\n---- After init\n");
    printf("1\n");

    void * mem[10];
    mem[0] = cpen212_alloc(heap_handle,0);
    assert(mem[0] == NULL);

    printf("2\n");
    mem[0] = cpen212_alloc(heap_handle, 3);
    printf("\n\n---- After alloc 3 bytes\n");
    assert(mem[0]);

    printf("3\n");
    mem[1] = cpen212_alloc(heap_handle, 50);
    printf("\n\n---- After alloc 50 bytes\n");
    assert(mem[1]);

    printf("4\n");
    mem[2] = cpen212_alloc(heap_handle, 200);
    printf("\n\n---- After alloc 200 bytes\n");
    assert(mem[2]);

    printf("4.5\n");
    mem[3] = cpen212_alloc(heap_handle, 3);
    printf("\n\n---- After unsuccesfully alloc 3 bytes\n");
    assert(mem[3] == NULL);

    //Should not be allocated
    printf("5\n");
    mem[3] = cpen212_alloc(heap_handle, 64);
    printf("\n\n---- After unsuccesfully alloc 64 bytes\n");
    assert(mem[3] == NULL);

    printf("6\n");
    cpen212_free(heap_handle, mem[2]);
    printf("\n\n---- After freeing 200 bytes\n");

    printf("7\n");
    mem[2] = cpen212_alloc(heap_handle, 200);
    printf("\n\n---- After alloc 200 bytes\n");
    assert(mem[2]);

    printf("8\n");
    mem[4] = cpen212_realloc(heap_handle,mem[2], 120);
    printf("\n\n---- After realloc 200 bytes to 120\n");
    assert(mem[4]);

    printf("8.5\n");
    mem[5] = cpen212_realloc(heap_handle, mem[1], 102);
    printf("\n\n---- After unsuccesful realloc 56 to 102(104) bytes\n");
    assert(mem[5] == NULL);

    printf("9\n");
    mem[4] = cpen212_realloc(heap_handle, mem[4], 180);
    printf("\n\n---- After realloc 120 bytes to 180\n");
    assert(mem[4]);


    printf("10\n");
    cpen212_free(heap_handle, mem[2] + 2);
    printf("\n\n---- After invalid address free\n");

    // this should fail
    printf("11\n");
    mem[5] = cpen212_alloc(heap_handle, 60000);
    printf("\n\n---- After unsuccessful alloc for 60000 bytes\n");
    assert(mem[5] == NULL);

    printf("12\n");
    //this should fail
    mem[6] = cpen212_realloc(heap_handle, mem[1],100);
    printf("\n\n---- After unsuccessfull realloc 100 bytes\n");
    assert(mem[6] == NULL);

    printf("12.5\n");
    cpen212_free(heap_handle, mem[4]);
    cpen212_free(heap_handle, mem[1]);
    mem[9]= cpen212_alloc(heap_handle, 250);
    printf("After freeing 200 bytes; \n then 56 bytes;\n then allocating 25o bytes.\n");
    assert(mem[9]);

    /*
    printf("13\n");
    for (int i = 0; i < 100; i++)
    {
       *((char*) mem[4] + i) = 0xA;
    }
   
    printf("14\n");
    printf("Rallocated content:\n");
    for (int i = 0; i < 100; i++)
    {
       printf("%x ", *((char*) mem[4] + i));
    }
    printf("\n");*/

    cpen212_free(heap_handle, mem[0]);
    printf("After freeing first 8 bytes\n\n");
    cpen212_free(heap_handle, mem[9]);
    printf("After freeing the 256 bytes\n\n");
    printf("Should have free heap now (one big 312 size block.)\n");

    mem[9] = cpen212_alloc(heap_handle, 300);
    printf("After allocating 300 bytes\n");
    assert(mem[9]);

    mem[9] = cpen212_realloc(heap_handle, mem[9],20);
    printf("After reallocating the 300 bytes to 20 bytes\n");
    assert(mem[9]);
    mem[8] = cpen212_alloc(heap_handle, 30);
    printf("After allocating 30 bytes\n");
    assert(mem[8]);
    mem[7] = cpen212_alloc(heap_handle, 30);
    printf("After allocating 30 bytes\n");
    assert(mem[7]);
    mem[6] = cpen212_alloc(heap_handle, 30);
    printf("After allocating another 30 bytes\n");
    assert(mem[6]);
    cpen212_free(heap_handle, mem[7]);
    printf("After freeing 30 bytes (pos 3/4)\n");
    cpen212_free(heap_handle, mem[6]);
    printf("After freeing 30 bytes (pos 4/4) should merge with 3/4\n");
    cpen212_free(heap_handle, mem[9]);
    printf("After freeing 20 bytes (pos 1/4)\n");
    cpen212_free(heap_handle, mem[8]);
    printf("After freeing 30 bytes (pos 2/4) should leave one big block.\n");
    mem[9] = cpen212_alloc(heap_handle, 300);
    printf("After allocating 300 bytes\n");
    assert(mem[9]);
    


    printf("\n---Task 5 tests---\n\n");

    cpen212_free(heap_handle, mem[9]);
    printf("After freeing 300 bytes\n");

    mem[0] = cpen212_alloc(heap_handle, 8);
    mem[1] = cpen212_alloc(heap_handle, 8);
    mem[2] = cpen212_alloc(heap_handle, 8);
    mem[3] = cpen212_alloc(heap_handle, 8);
    mem[4] = cpen212_alloc(heap_handle, 8);
    mem[5] = cpen212_alloc(heap_handle, 8);
    mem[6] = cpen212_alloc(heap_handle, 8);
    mem[7] = cpen212_alloc(heap_handle, 8);
    mem[8] = cpen212_alloc(heap_handle, 8);
    mem[9] = cpen212_alloc(heap_handle, 8);
    assert(mem[0]);
    assert(mem[1]);
    assert(mem[2]);
    assert(mem[3]);
    assert(mem[4]);
    assert(mem[5]);
    assert(mem[6]);
    assert(mem[7]);
    assert(mem[8]);
    assert(mem[9]);

    printf("After allocating 10 8 byte blocks\n");

    cpen212_free(heap_handle, mem[9]);
    mem[8] = cpen212_realloc(heap_handle, mem[8], 40);
    printf("After reallocating mem[8] to 40 bits, should extend forward\n");
    assert(mem[8]);

    cpen212_free(heap_handle, mem[8]);
    cpen212_free(heap_handle, mem[6]);
    mem[7] = cpen212_realloc(heap_handle, mem[7], 64);
    assert(mem[7]);
    printf("After freeing 8 and 6 and then reallocating mem[7], should move backwards and coalesce with 6 and 8.\n");


    mem[7] = cpen212_realloc(heap_handle, mem[7], 32);
    assert(mem[7]);
    printf("After reallocating 7 to smaller size, should combine with forward block.\n");

    mem[8] = cpen212_alloc(heap_handle, 16);
    assert(mem[8]);
    printf("After allocating 16 more bytes.\n");

    cpen212_free(heap_handle, mem[2]);
    mem[1] = cpen212_realloc(heap_handle, mem[1],16);
    printf("After freeing mem[2] and re allocating 1 into two; should extend forward\n"); 

    cpen212_free(heap_handle, mem[0]);
    void* m1 = mem[1];
    mem[0] = cpen212_realloc(heap_handle, mem[1], 8);
    assert(m1 == mem[1]);
    assert(mem[0]);
    printf("After freeing the first one and then shrinking the one after; should coalesce with previous one.\n");

    mem[1] = cpen212_alloc(heap_handle, 0x28);
    assert(mem[1]);
    mem[1] = cpen212_realloc(heap_handle, mem[1], 32);
    assert(mem[1]);
    mem[1] = cpen212_realloc(heap_handle,mem[1], 40);
    assert(mem[1]);
    mem[1] = cpen212_realloc(heap_handle, mem[1], 8);
    assert(mem[1]);
    printf("Now\n");
    mem[1] = cpen212_realloc(heap_handle, mem[1], 40);
    assert(mem[1]);
    *((int*)((uintptr_t)mem[1])) = 1;
    *((int*)((uintptr_t)mem[1] + 8)) = 2;
    *((int*)((uintptr_t)mem[1] + 16)) = 3;
    *((int*)((uintptr_t)mem[1] + 24)) = 4;
    *((int*)((uintptr_t)mem[1] + 32)) = 5; 

    cpen212_free(heap_handle, mem[0]);
    cpen212_realloc(heap_handle, mem[1],48); 



    cpen212_free(heap_handle + 4, mem[9]);
    /*
    //cpen212_debug(heap_handle,1);
    //assert(cpen212_debug(heap_handle,0));
    
    void * mem[10];
    mem[0] = cpen212_alloc(heap_handle,0);
    assert(mem[0] == NULL);

    printf("2");
    mem[0] = cpen212_alloc(heap_handle, 3);
    printf("\n\n---- After alloc 3 bytes\n");
    //cpen212_debug(heap_handle,1);
    //assert(cpen212_debug(heap_handle,0));
    assert(mem[0]);

    printf("3");
    mem[1] = cpen212_alloc(heap_handle, 19);
    printf("\n\n---- After alloc 19 bytes\n");
    //cpen212_debug(heap_handle,1);
    //assert(cpen212_debug(heap_handle,0));
    assert(mem[1]);

    printf("4");
    mem[2] = cpen212_alloc(heap_handle, 63800);
    printf("\n\n---- After alloc 63800 bytes\n");
    //cpen212_debug(heap_handle,1);
    //assert(cpen212_debug(heap_handle,0));
    assert(mem[2]);

    printf("5");
    mem[3] = cpen212_alloc(heap_handle, 24);
    printf("\n\n---- After alloc 24 bytes\n");
    //cpen212_debug(heap_handle,1);
    //assert(cpen212_debug(heap_handle,0));
    assert(mem[3]);

    printf("6");
    cpen212_free(heap_handle, mem[2]);
    printf("\n\n---- After free 63800 bytes\n");
    //cpen212_debug(heap_handle,1);
    //assert(cpen212_debug(heap_handle,0));
    
    printf("7");
    mem[4] = cpen212_alloc(heap_handle, 8001);
    printf("\n\n---- After alloc 8001 bytes\n");
    //cpen212_debug(heap_handle,1);
    //assert(cpen212_debug(heap_handle,0));
    assert(mem[4]);

    printf("8");
    // this should fail
    mem[5] = cpen212_alloc(heap_handle, 60000);
    printf("\n\n---- After alloc req for 60000 bytes\n");
    //cpen212_debug(heap_handle,1);
    //assert(cpen212_debug(heap_handle,0));
    assert(mem[5] == NULL);

    printf("9");
    cpen212_free(heap_handle, mem[2] + 2);
    printf("\n\n---- After invalid address free\n");
    //cpen212_debug(heap_handle,1);
    //assert(cpen212_debug(heap_handle,0));
    
    printf("10");
    mem[6] = cpen212_realloc(heap_handle, mem[1],19);
    printf("\n\n---- After realloc 19 bytes\n");
    //cpen212_debug(heap_handle,1);
    //assert(cpen212_debug(heap_handle,0));
    assert(mem[6]);

    printf("11");
    for (int i = 0; i < 19; i++)
    {
       *((char*) mem[6] + i) = 0xA;
    }
    printf("12");
    mem[7] = cpen212_realloc(heap_handle, mem[6],50);
    printf("\n\n---- After realloc expand to 50 bytes bytes\n");
    //cpen212_debug(heap_handle,1);
    //assert(cpen212_debug(heap_handle,0));
    assert(mem[7]);

    printf("13");
    printf("Rallocated content:\n");
    for (int i = 0; i < 19; i++)
    {
       printf("%x ", *((char*) mem[7] + i));
    }*/

    return 0;
}
