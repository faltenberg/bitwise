#include "sbuffer.h"

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>


#define MAX(a, b) ( (a) >= (b) ? (a) : (b) )


void* __bufGrow(const void* buffer, size_t newLength, size_t elementSize) {
  size_t newCapacity = MAX(1 + 2*bufCapacity(buffer), newLength);
  assert(newCapacity >= newLength);
  size_t newSize = newCapacity * elementSize + offsetof(BufHeader, bytes);
  BufHeader* newHeader;
  if (buffer) {
    newHeader = (BufHeader*) realloc(__bufHeader(buffer), newSize);
  } else {
    newHeader = (BufHeader*) malloc(newSize);
    newHeader->length = 0;
  }
  newHeader->capacity = newCapacity;
  return newHeader->bytes;
}
