#ifndef __CPEN212COMMON_H__
#define __CPEN212COMMON_H__

// YOUR CODE HERE
//
// This file is included in cpen212alloc.c and cpen212debug.c,
// so it would be the right place to define data types they both share.

typedef struct {
      void* heap_start;
      void* heap_end;
}heap_header;

typedef struct block_header{
      bool alloc;
      struct block_header* next;
      size_t storage;
}block_header;

#endif // __CPEN212COMMON_H__
