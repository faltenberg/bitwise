#ifndef __STRINTERN_H__
#define __STRINTERN_H__


/**
 * String Interning
 * ================
 *
 * Given a string of characters string interning stores it in an internal buffer and returns the
 * internal `string`. If a string does already exists in the buffer, no new data is stored and the
 * already interned `string` is returned. A substring of an existing string is not interned either
 * but instead the interned `string` is returned. If a string is to be interned and there does
 * exist an interned substring, then the new string is interned while the substring still remains
 * interned. Otherwise all the copies of the interned substring would become invalid. Since the
 * substring was interned prior to the larger string, a consecutive interning of the substring will
 * return the interned substring and not a substring of the larger string. Substrings are not
 * `'\0'` terminated!
 *
 * String interning reduces the memory usage if the program works a lot with identical strings
 * (e.g. commands from a UI) and reduces string comparison to a simple pointer comparison.
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
 *   string a = strintern("abc");          // interns and returns internal "abc"
 *   assert(a.chars != "abc");             // "abc" resides in the data segment and is not interned
 *
 *   string b = strintern("abc");          // returns internal "abc"
 *   assert(a.chars == b.chars);           // no need for costly strcmp()
 *
 *   string c = strintern("ab");           // returns substring "ab" to internal "abc"
 *   assert(c.chars == a.chars);           // memory is reused for substring
 *   assert(c.len != a.len);               // only the length differs
 *   assert(!STREQ(a, c));                 // compares with substring support
 *   assert(c.chars[c.len] != '\0');       // subsring is not terminated!
 *
 *   const char s[] = {'_', 'a', 'b', 'c', '_'};
 *   string d = strinternRange(s+1, s+4);  // returns internal "abc"
 *   assert(STREQ(a, d));
 *
 *   string e = strintern("_abc_");        // interns and returns internal "_abc_"
 *   string f = strintern("abc");          // returns internal "abc" since it was interned first
 *   assert(STREQ(a, f));                  // thus no substring to "_abc_"
 *
 *   string g = strinternRange(e.chars+1, e.chars+3);  // returns substring "ab" to internal "abc"
 *   assert(STREQ(c, g));                  // both are substrings of internal "abc"
 *
 *   strinternFree();                      // releases internal memory
 * }
 * ```
 */


#include "str.h"


/**
 * The `STREQ()` macro expands to a string comparison by pointer and length comparison.
 * This condition correctly checks substrings.
 *
 * - **param:** `a` - the first string for comparison
 * - **param:** `b` - the second string for comparison
 */
#define STREQ(a, b) (a.chars == b.chars && a.len == b.len)


/**
 * `strintern()` stores a string in an internal buffer and returns it. If there is already an
 * interned equal string, then it is returned instead and no interning takes place. If the given
 * string can be found within an already interned string a substring to that interned string is
 * returned. The substring will be not ``\0`` terminated.
 *
 * - **param:** `string` - the string to be interned
 * - **return:** the interned string
 */
string strintern(const char* string);


/**
 * `strinternRange()` stores a string within a given range. It is useful when substrings from a
 * large string shall be interned. The entire string can be interned by calling
 * `strinternRange(s, s+strlen(s))`.
 *
 * - **param:** `start` - the start of the string to be interned
 * - **param:** `end`   - the end of the string to be interned
 * - **return:** the interned string
 */
string strinternRange(const char* start, const char* end);


/**
 * `strinternFree()` deallocates all interned strings and releases the memory. You should not
 * forget to call it as it will lead to memory leaks otherwise.
 */
void strinternFree();


#endif  // __STRINTERN_H__
