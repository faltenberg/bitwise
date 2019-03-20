#ifndef __STR_H__
#define __STR_H__


/**
 * String
 * ======
 *
 * Since strings in C are quite limited this header provides a small wrapper. The goal is not to
 * replace the C `string.h` functionality, though. `string`s are not guaranteed to be '\0'
 * terminated! One can say `string` is just a window into a C string, given by a pointer and some
 * non-negative length. This makes creating substrings very fast.
 *
 *
 * Example
 * -------
 *
 * ```c {.line-numbers}
 * #include "str.h"
 * #include <assert.h>
 * #include <stdio.h>
 *
 * int main(int argc, char** argv) {
 *   string option = fromCString(argv[1]);       // wrap C string
 *   if (streq(option, "-h")) {                  // comparison with C string
 *     // print help menu
 *   }
 *
 *   const char* lorem = "Lorem ipsum dolor";
 *   string s = fromRange(lorem+6, lorem+11);    // substring not '\0' terminated!
 *   assert(s.chars == lorem+6);
 *   assert(s.len == 5);
 *   assert(cstrequal(s, "ipsum"));              // compare with C string
 *   assert(strequal(s, fromCString("ipsum")));  // compare strings
 *
 *   for (int i = 0; i < s.len; i++) {
 *     char c = s.chars[i];                      // still easy access
 *     // do something
 *   }
 *
 *   printf("s: %.*s\n", s.len, s.chars);        // should always print like that
 * }
 * ```
 */


#include <stdbool.h>


/**
 * `string` wraps a C string in a struct together with its length.
 *
 * - **field:** `chars` - the pointer to the characters
 * - **field:** `len`   - the length of the string
 */
typedef struct string {
  const char* chars;
  int         len;
} string;


/**
 * `fromCString()` sreates a new string from a C string.
 *
 * - **param:** `s` - the C string
 * - **return:** the wrapped string
 */
string fromCString(const char* s);


/**
 * `fromRange()` creates a substring in a given range of some C string. If `start > end` then the
 * length will be `0`, but the string will still point to `start`.
 *
 * - **param:** `start` - the pointer to the start of the substring
 * - **param:** `end`   - the pointer to the end of the substring
 * - **return:** the substring from `start` to `end`
 */
string fromRange(const char* start, const char* end);


/**
 * `strequal()` compares two strings for equality which means that they have the same length and
 * characters.
 *
 * - **param:** `a` - the first string
 * - **param:** `b` - the second string
 * - **return:** `true` if both strings are equal
 */
bool strequal(string a, string b);


/**
 * `cstrequal()` compares a string to a C string for equality which means they have same length and
 * characters.
 *
 * - **param:** `a` - the string
 * - **param:** `b` - the C string
 * - **return:** `true` if both strings are equal
 */
bool cstrequal(string a, const char* b);


#endif  //  __STR_H__
