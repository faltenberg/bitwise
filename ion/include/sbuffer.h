#ifndef __SBUFFER_H__
#define __SBUFFER_H__

#include <stddef.h>
#include <stdlib.h>


/**
 * Stretchy Buffer
 * ===============
 *
 * Stretchy buffers, invented by Sean Barrett, are an approach for dynamic arrays that have the
 * same usage as normal C arrays, but grows and shrinks if necessary. There is no special struct
 * to use. Stretchy buffers use a header inside the array data to store length information.
 * The memory for a buffer of integers does look like this:
 *
 * ```
 *             0x0100[ length    ]
 *             0x0104[ capacity  ]
 *   buffer -> 0x0108[ data[0]   ]
 *                |
 *             0x01f0[ data[n-1] ]
 * ```
 *
 * Example
 * -------
 *
 * ```c
 * #include "sbuffer.h"
 *
 * int main() {
 *   int* buffer = NULL;   // the NULL assignment is essential!
 *   bufPush(buffer, 42);  // (re-)allocates memory if necessary and stores value at the end
 *
 *   for (int i = 0; i < bufLength(buffer); i++) {  // buffers have a length information
 *     printf("[%d]: %d\n", i, buffer[i]);          // same access as with C arrays
 *   }
 *
 *   bufFree(buffer);      // deallocates the memory and sets buffer to NULL
 * }
 * ```
 */


typedef struct BufHeader {
  size_t length;
  size_t capacity;
  char   bytes[0];
} BufHeader;


#define __bufHeader(b) ( (BufHeader*) ((char*) (b) - offsetof(BufHeader, bytes)) )

#define bufLength(b) ( (b) ? __bufHeader(b)->length : 0 )

#define bufCapacity(b) ( (b) ? __bufHeader(b)->capacity : 0 )

#define __bufFits(b, n) ( bufLength(b) + (n) <= bufCapacity(b) )

#define bufFit(b, n) ( __bufFits(b, n) ? 0 : ((b) = __bufGrow(b, bufLength(b)+(n), sizeof(*(b)))) )

#define bufPush(b, x) ( bufFit(b, 1), (b)[__bufHeader(b)->length++] = (x) )

#define bufFree(b) ( (b) ? (free(__bufHeader(b)), (b) = NULL) : 0 )


void* __bufGrow(const void* buffer, size_t newLength, size_t elementSize);


#endif  // __SBUFFER_H__
