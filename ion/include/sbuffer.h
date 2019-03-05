#ifndef __SBUFFER_H__
#define __SBUFFER_H__


/**
 * Stretchy Buffer
 * ===============
 *
 * Stretchy buffers, invented by Sean Barrett, are an approach for dynamic arrays that have the
 * same usage as normal C arrays, but grow if neccessary. There is no special struct to use.
 * Stretchy buffers use a header inside the array data to store the length information. They are
 * used just like normal arrays and as such can be passed around to any library. If the buffer
 * exceeds its capacity it will be reallocated and expanded. The new address will be saved in the
 * variable holding the pointer to the buffer. The buffer will double its capacity every time it
 * grows. This provides a linear amortized complexity. The buffer will never shrink which might
 * be inconvinient in some programs.
 *
 * A declared buffer must be initialized with `NULL`! Otheriwse the macros cannot distinguish it
 * from existing buffers. The delete macro will assign `NULL` to the buffer variable automatically.
 *
 *
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
 *
 * Example
 * -------
 *
 * ```c {.line-numbers}
 * #include "sbuffer.h"
 * #include <assert.h>
 *
 * int main() {
 *   // expanded to int* buffer = NULL;
 *   SBUF(int) buffer = NULL;  // the NULL assignment is essential!
 *   bufPush(buffer, 42);      // (re-)allocates memory if necessary and stores value at the end
 *   bufFit(buffer, 10);       // optional: reserves memory for 10 more items
 *
 *   assert(buffer != NULL);
 *   assert(bufLength(buffer) == 1);
 *   assert(bufCapacity(buffer) == 11);
 *
 *   for (size_t i = 0; i < bufLength(buffer); i++) {
 *     printf("[%zu]: %d\n", i, buffer[i]);  // same access as with C arrays
 *   }
 *
 *   for (int* it = buffer; it != bufEnd(buffer); it++) {
 *     printf("%d\n", *it);  // C++ like iteration is possible
 *   }
 *
 *   bufFree(buffer);  // deallocates the memory and sets buffer to NULL
 * }
 * ```
 */


#include <stddef.h>
#include <stdlib.h>


/**
 * **INTERNAL!** Stores the length information of the buffer. `bytes` is an anker that is used
 * to access the buffer data. It doesn't count into `sizeof(BufHeader)` as its size is `0` and is
 * only increased during runtime.
 *
 * **field:** `length` - the current length of the buffer
 * **field:** `capacity` - the capacity of the buffer
 * **field:** `bytes` - the buffer data
 */
typedef struct BufHeader {
  size_t length;
  size_t capacity;
  char   bytes[0];
} BufHeader;


/**
 * The `SBUF()` macro expands to the pointer type of the provided base type. It exists merely
 * to visualize that a variable is not simply a pointer, but a stretchy buffer.
 *
 * - **param:** `x` - the base type of the buffer
 */
#define SBUF(x) x*


/**
 * **INTERNAL!** Expands to the address of the buffer header given a pointer to the buffer data.
 *
 * - **param:** - `b` the pointer to the buffer's data
 */
#define __bufHeader(b) ( (BufHeader*) ((char*) (b) - offsetof(BufHeader, bytes)) )


/**
 * The `bufLength()` macro expands to the length of a buffer.
 *
 * - **param:** `b` - the pointer to a buffer
 */
#define bufLength(b) ( (b) ? __bufHeader(b)->length : 0 )

/**
 * The `bufCapacity()` macro expands to the capacity of a buffer.
 *
 * - **param:** `b` - the pointer to a buffer
 */
#define bufCapacity(b) ( (b) ? __bufHeader(b)->capacity : 0 )


/**
 * **INTERNAL!** Expands to a condition that checks if the buffer can hold `n` more items.
 *
 * - **param:** `b` - the pointer to a buffer
 * - **param:** `n` - the number of elements that the buffer shall fit
 */
#define __bufFits(b, n) ( bufLength(b) + (n) <= bufCapacity(b) )


/**
 * Expands to an assignment statement which will grow the buffer if it is too small to fit more
 * items. Otherwise the macro expands to an empty statement.
 *
 * - **param:** `b` - the pointer to a buffer
 * - **param:** `n` - the number of elements that the buffer shall fit
 */
#define bufFit(b, n) ( __bufFits(b, n) ? 0 : ((b) = __bufGrow(b, bufLength(b)+(n), sizeof(*(b)))) )


/**
 * Expands to an statement that grows the buffer if necessary and inserts an element at the end.
 * This macro does push one single element! The `__VA_ARGS__` are used for some special data
 * types that need extra treatment on the client's side otherwise.
 *
 * - **param:** `b` - the pointer to a buffer
 * - **param:** `...` - the element that shall be pushed to the buffer
 */
#define bufPush(b, ...) ( bufFit(b, 1), (b)[__bufHeader(b)->length++] = (__VA_ARGS__) )


/**
 * Expands to the address of the memory past the end of the buffer. This can be used for a C++
 * iteration.
 *
 * - **param:** `b` - the pointer to a buffer
 */
#define bufEnd(b) ( (b) + bufLength(b) )


/**
 * Expands to a statement that frees the buffer's memory and assigns `NULL` to a buffer variable.
 *
 * - **param:** `b` - the pointer to a buffer
 */
#define bufFree(b) ( (b) ? (free(__bufHeader(b)), (b) = NULL) : 0 )


/**
 * **INTERNAL!** Checks if a buffer is large enough or shall be grown in capacity. If so the buffer
 * will be reallocated and all data will be copied to the new memory location, whose address will
 * then be returned.
 *
 * - **param:** `buffer` - the pointer to a buffer
 * - **param:** `newLength` - the minimal number of elements that the buffer shall fit
 * - **param:** `elementSize` - the size of one single element in the buffer
 * - **return:** the pointer to the new memory location
 */
void* __bufGrow(const void* buffer, size_t newLength, size_t elementSize);


#endif  // __SBUFFER_H__
