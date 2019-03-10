#include "arena.h"

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

/**
 * Arch: 64bit
 * L1i:  32KB
 * L1:   32KB
 * L2:  256KB
 * L3:    8MB
 * Page Size:  4KB
 * Line Size: 64B
 */

#define CACHE_PAGE_SIZE 4*1024
#define CACHE_LINE_SIZE 64

#define MAX(x, y) ((x) >= (y) ? (x) : (y))

#define ALIGN_DOWN(n, a) ((n) & ~((a) - 1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n) + (a) - 1, (a))
#define ALIGN_DOWN_PTR(p, a) ((void *)ALIGN_DOWN((uintptr_t)(p), (a)))
#define ALIGN_UP_PTR(p, a) ((void *)ALIGN_UP((uintptr_t)(p), (a)))
#define ARENA_ALIGNMENT 8
#define ARENA_BLOCK_SIZE CACHE_PAGE_SIZE


/**
 * Returns the address where `size` bytes fit. The addresss will be aligned to a multiple
 * of `size`.
 *
 * 1. allocate block to fit CACHE_PAGE_SIZE (7KB block will have aligned 4KB block)
 * 2. check if current cache line can fit new data
 * 3.
 * 4B=2^2 align at 0x00 0x04 0x08 0x0c
 * 7B=2^3-1=8-1
 *  01234567
 * |        |
 *  |       |
 */
// TODO: implement and rename
void* arena_alloc(Arena* arena, size_t size) {
  void* ptr =  malloc(size);
  arena->ptr = ptr;
  arena->end = ptr + size;
  arena->totalSpace += size;
  arena->usedSpace += size;
  bufPush(arena->blocks, arena->ptr);
  return ptr;
}


void arenaFree(Arena *arena) {
  for (void** it = arena->blocks; it != bufEnd(arena->blocks); it++) {
    free(*it);
  }
  bufFree(arena->blocks);
  arena->ptr = NULL;
  arena->end = NULL;
  arena->totalSpace = 0;
  arena->usedSpace = 0;
}


// TODO: delete
#include <unistd.h>
void arena_grow(Arena* arena, size_t min_size) {
    size_t size = ALIGN_UP(MAX(ARENA_BLOCK_SIZE, min_size), ARENA_ALIGNMENT);
    arena->ptr = malloc(1024);
    arena->end = arena->ptr + size;
    bufPush(arena->blocks, arena->ptr);
}
void* arenaAlloc(Arena *arena, size_t size) {
    if (size > (size_t)(arena->end - arena->ptr)) {
        arena_grow(arena, size);
        assert(size <= (size_t)(arena->end - arena->ptr));
    }
    void *ptr = arena->ptr;
    arena->ptr = ALIGN_UP_PTR(arena->ptr + size, ARENA_ALIGNMENT);
    assert(arena->ptr <= arena->end);
    assert(ptr == ALIGN_DOWN_PTR(ptr, ARENA_ALIGNMENT));
    return ptr;
}
