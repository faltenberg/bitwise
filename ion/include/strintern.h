#ifndef __STRINTERN_H__
#define __STRINTERN_H__


/**
 * String Interning
 * ================
 *
 * Given a string of characters string interning stores it in an internal buffer and returns the
 * pointer to that internal string. If a string does already exists in the buffer, no new data is
 * stored and the pointer to the already stored string is returned. This approach reduces the
 * memory usage if the program works with a lot with identical strings (e.g. commands from a UI)
 * and reduces string comparison to a simple pointer comparison.
 *
 *
 * Example
 * -------
 *
 * ```c {.line-numbers}
 * #include "strintern.h"
 * #include <assert.h>
 *
 * int main() {
 *   const char* a = strintern("abc");  // the string "abc" is stored in an internal buffer
 *   const char* b = strintern("abc");  // the pointer to stored "abc" is returned
 *   const char c[] = {'a', 'b', 'c', 'd'};
 *   c = strinternRange(c, c+3);        // the pointer to stored "abc" is returned
 *
 *   assert(a == b);                    // no need for costly strcmp()
 *   assert(a == c);
 *   assert(a != "abc");                // "abc" resides in the data segment, is not interned
 *
 *   strinternFree();                   // release internal memory
 * }
 * ```
 */


/**
 * Stores `string` in an internal buffer and return that address. If there is already an interned
 * equal string, then its address is returned and no interning takes place.
 *
 * - **param:** `string` - the string to be interned
 * - **return:** the address of the interned string
 */
const char* strintern(const char* string);


/**
 * Stores a string in a given range. It is useful when substrings from a large string shall be
 * interned. The entire string can be interned by calling `strinternRange(s, s+strlen(s))`.
 * The interned substring will be terminated with `'\0'`.
 *
 * - **param:** `start` - the pointer to the first character
 * - **param:** `end` - the pointer to the last character
 * - **return:** the address of the interned string
 */
const char* strinternRange(const char* start, const char* end);


/**
 * Deallocates all interned strings and releases the memory. You should not forget to call it as
 * it will lead to memory leaks otherwise.
 */
void strinternFree();


#endif  // __STRINTERN_H__
