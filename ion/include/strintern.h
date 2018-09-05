#ifndef __STRINTERN_H__
#define __STRINTERN_H__


/**
 * String Interning
 * ================
 *
 * Given a string of characters string interning stores it in an internal buffer and returns the
 * pointer to that internal string. If a string does already exists in the buffer, no new data is
 * stored and the pointer to the already stored string is returned. This approach reduces the
 * memory usage if the program works a lot with identical strings (e.g. commands from a UI) and
 * reduces string comparison to a simple pointer comparison.
 *
 * Example
 * -------
 *
 * ```c
 * #include "strintern.h"
 * #include <assert.h>
 *
 * int main() {
 *   const char* a = strintern("abc");  // the string "abc" is stored in an internal buffer
 *   const char* b = strintern("abc");  // the pointer to stored "abc" is returned
 *   const char c[] = {'a', 'b', 'c'};
 *   c = strintern(c);                  // the pointer to stored "abc" is returned
 *   assert(a == b);                    // no need for costly strcmp()
 *   assert(a == c);
 *   assert(a != "abc");                // "abc" resides in the data segment, is not interned
 * }
 * ```
 */


const char* strintern(const char* string);

const char* strinternRange(const char* start, const char* end);


#endif  // __STRINTERN_H__
