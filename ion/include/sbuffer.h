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
 *   // expands to int* buffer = NULL;
 *   SBUF(int) buffer = NULL;  // the NULL assignment is essential!
 *   assert(sbufLength(buffer) == 0);
 *   assert(sbufCapacity(buffer) == 0);
 *
 *   sbufPush(buffer, 42);  // (re-)allocates memory if necessary and stores value at the end
 *   assert(buffer != NULL);
 *   assert(sbufLength(buffer) == 1);
 *   assert(sbufCapacity(buffer) == 1);
 *
 *   sbufFit(buffer, 10);  // reserves memory for 10 more items
 *   assert(sbufLength(buffer) == 1);
 *   assert(sbufCapacity(buffer) == 11);
 *
 *   for (size_t i = 0; i < sbufLength(buffer); i++) {
 *     printf("[%zu]: %d\n", i, buffer[i]);  // same access as with C arrays
 *   }
 *
 *   for (int* it = buffer; it != sbufEnd(buffer); it++) {
 *     printf("%d\n", *it);  // C++ like iteration is possible
 *   }
 *
 *   sbufFree(buffer);  // deallocates the memory and sets buffer to NULL
 *   assert(buffer == NULL);
 *   assert(sbufLength(buffer) == 0);
 *   assert(sbufCapacity(buffer) == 0);
 * }
 * ```
 */


#include <stddef.h>
#include <stdlib.h>


/**
 * **INTERNAL!** `BufHeader` stores the length information of the buffer. `bytes` is an anker that
 * is used to access the buffer data. It doesn't count into `sizeof(BufHeader)` as its size is `0`
 * and is only increased during runtime.
 *
 * - **field:** `length`   - the current length of the buffer
 * - **field:** `capacity` - the capacity of the buffer
 * - **field:** `bytes`    - the buffer data
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
 * **INTERNAL!** The `__sbufHeader()` macro expands to the address of the buffer header given a
 * pointer to the buffer data.
 *
 * - **param:** - `b` the pointer to the buffer's data
 */
#define __sbufHeader(b) ( (BufHeader*) ((char*) (b) - offsetof(BufHeader, bytes)) )


/**
 * The `sbufLength()` macro expands to the length of a buffer.
 *
 * - **param:** `b` - the pointer to a buffer
 */
#define sbufLength(b) ( (b) ? __sbufHeader(b)->length : 0 )


/**
 * The `sbufCapacity()` macro expands to the capacity of a buffer.
 *
 * - **param:** `b` - the pointer to a buffer
 */
#define sbufCapacity(b) ( (b) ? __sbufHeader(b)->capacity : 0 )


/**
 * **INTERNAL!** The `__sbufFits()` macro expands to a condition that checks if the buffer can hold
 * `n` more items.
 *
 * - **param:** `b` - the pointer to a buffer
 * - **param:** `n` - the number of elements that the buffer shall fit
 */
#define __sbufFits(b, n) ( sbufLength(b) + (n) <= sbufCapacity(b) )


/**
 * The `bufFit()` macro expands to an assignment statement which will grow the buffer if it is too
 * small to fit more items. Otherwise the macro expands to an empty statement.
 *
 * - **param:** `b` - the pointer to a buffer
 * - **param:** `n` - the number of elements that the buffer shall fit
 */
#define sbufFit(b, n) ( __sbufFits(b, n) ? 0 : ((b) = __sbufGrow(b, sbufLength(b)+(n), sizeof(*(b)))) )


/**
 * The `sbufPush()` macro expands to an statement that grows the buffer if necessary and inserts
 * an element at the end. This macro does push one single element! The `__VA_ARGS__` are used for
 * some special data* types that need extra treatment on the client's side otherwise.
 *
 * - **param:** `b`   - the pointer to a buffer
 * - **param:** `...` - the element that shall be pushed to the buffer
 */
#define sbufPush(b, ...) ( sbufFit(b, 1), (b)[__sbufHeader(b)->length++] = (__VA_ARGS__) )


/**
 * The `sbufEnd()` macro expands to the address of the memory past the end of the buffer. This can
 * be used for a C++ iteration.
 *
 * - **param:** `b` - the pointer to a buffer
 */
#define sbufEnd(b) ( (b) + sbufLength(b) )


/**
 * The `sbufFree()` macro expands to a statement that frees the buffer's memory and assigns `NULL`
 * to a buffer variable.
 *
 * - **param:** `b` - the pointer to a buffer
 */
#define sbufFree(b) ( (b) ? (free(__sbufHeader(b)), (b) = NULL) : 0 )


/**
 * **INTERNAL!** `__sbufGrow()` checks if a buffer is large enough or shall be grown in capacity.
 * If so the buffer will be reallocated and all data will be copied to the new memory location,
 * whose address will then be returned.
 *
 * - **param:** `buffer`      - the pointer to a buffer
 * - **param:** `newLength`   - the minimal number of elements that the buffer shall fit
 * - **param:** `elementSize` - the size of one single element in the buffer
 * - **return:** the pointer to the new memory location
 */
void* __sbufGrow(const void* buffer, size_t newLength, size_t elementSize);


#endif  // __SBUFFER_H__
