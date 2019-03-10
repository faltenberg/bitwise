#ifndef __ARENA_H__
#define __ARENA_H__

#include "sbuffer.h"


typedef struct Arena {
  void*       ptr;
  void*       end;
  SBUF(void*) blocks;
  int         totalSpace;
  int         usedSpace;
} Arena;


void* arenaAlloc(Arena* arena, size_t size);
void* arena_alloc(Arena* arena, size_t size);  // TODO: delete

void arenaFree(Arena* arena);


#endif  // __ARENA_H__
