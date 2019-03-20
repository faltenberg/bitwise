#include "sbuffer.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>


#define MAX(a, b) ( (a) >= (b) ? (a) : (b) )


void* __sbufGrow(const SBUF(void) buffer, size_t newLength, size_t elementSize) {
  size_t newCapacity = MAX(1 + 2*sbufCapacity(buffer), newLength);
  size_t newSize = newCapacity * elementSize + offsetof(BufHeader, bytes);

  assert(sbufCapacity(buffer) <= (SIZE_MAX - 1)/2);
  assert(newCapacity <= (SIZE_MAX - offsetof(BufHeader, bytes))/elementSize);
  assert(newCapacity >= newLength);

  BufHeader* newHeader;
  if (buffer) {
    newHeader = (BufHeader*) realloc(__sbufHeader(buffer), newSize);
  } else {
    newHeader = (BufHeader*) malloc(newSize);
    newHeader->length = 0;
  }

  newHeader->capacity = newCapacity;
  return newHeader->bytes;
}
