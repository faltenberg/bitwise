#ifndef __STR_H__
#define __STR_H__


/**
 * String
 * ======
 *
 * Since strings in C are quite limited this header provides a small wrapper. The goal is not to
 * replace the C *string.h* functionality, though. `string`s are not guaranteed to be `'\0'`
 * terminated! One can say `string` is just a window into a C string, given by a pointer and some
 * non-negative length. This makes creating substrings very fast. String can have the ownership
 * over the underlying memory, if some memory was allocated during the creation process. When
 * working with static C strings, the wrapper will never get the ownership, nor will a wrapper
 * that points into a range of some other string. If a string was created as a copy of some string
 * then it has the ownership over the memory and must be freed! Make sure all substring's are not
 * in use anymore before deleting the underlying memory.
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
 *   string option = stringFromArray(argv[1]);  // wrap C string
 *   if (cstrequal(option, "-h")) {             // comparison with C string
 *     // print help menu
 *   }
 *   strFree(option);  // optional, since option is not owning memory
 *   assert(option.len == 0);
 *   assert(option.chars == "");
 *
 *   const char* lorem = "Lorem ipsum dolor";
 *   string s = stringFromRange(lorem+6, lorem+11);  // substring not '\0' terminated!
 *   assert(s.chars == lorem+6);
 *   assert(s.len == 5);
 *   assert(cstrequal(s, "ipsum"));                  // compare with C string
 *   assert(strequal(s, stringFromArray("ipsum")));  // compare strings
 *   printf("s: %.*s\n", s.len, s.chars);            // should always print like that
 *
 *   string p = stringFromPrint("num: %d", 42);  // a versatile way to create strings
 *   for (int i = 0; i < p.len; i++) {
 *     char c = p.chars[i];  // still easy access
 *     // do something
 *   }
 *   if (p.owned) {  // p had allocated memory and must be freed
 *     strFree(p.chars);
 *   }
 *   assert(p.len == 0);
 *   assert(p.chars == "");
 * }
 * ```
 */


#include <stdbool.h>


/**
 * `string` wraps a C string in a struct together with its length.
 *
 * - **field:** `len`   - the length of the string
 * - **field:** `owned` - the string memory ownership
 * - **field:** `chars` - the pointer to the characters
 */
typedef struct string {
  int         len;
  bool        owned;
  const char* chars;
} string;


/**
 * `stringFromArray()` creates a new string from a C string.
 *
 * - **param:** `s` - the C string
 * - **return:** the wrapped string
 */
string stringFromArray(const char* s);


/**
 * `stringFromRange()` creates a substring in a given range of some C string. If `start > end`
 * then the length will be `0`, but the string will still point to `start`.
 *
 * - **param:** `start` - the pointer to the start of the substring
 * - **param:** `end`   - the pointer to the end of the substring
 * - **return:** the substring from `start` to `end`
 */
string stringFromRange(const char* start, const char* end);


/**
 * `stringFromPrint()` returns a string that is generated just like `printf()` from a format string
 * and the arguments it specifies. The returned string will have the ownership over the allocated
 * memory and must be freed at some point.
 *
 * - **param:** `format` - the format string
 * - **param:** `...`    - the arguments specified by the format string
 * - **return:** the formated string
 */
string stringFromPrint(const char* format, ...);


/**
 * `strFree()` releases the string possesses the ownership over its memory. In every case, the
 * string's content is replaced by an empty string with length 0.
 *
 * - **param:** `s` - the string to be freed
 */
void strFree(string* s);


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
