#ifndef __LOC_H__
#define __LOC_H__


/**
 * Location
 * ========
 *
 * `Location` is a simple wrapper for a character's location within a source code. A location has
 * the line (starting with 1) within the source and a position (starting with 1) within the line.
 *
 *
 * Example
 * -------
 *
 * ```c {.line-numbers}
 * #include "loc.h"
 * #include "token.h"
 * #include <stdio.h>
 *
 * int main() {
 *   Token t;  // get a token from somewhere
 *   Location loc = t.start;
 *   printf("location: %d:%d\n", loc.line, loc.pos);
 * }
 * ```
 */


/**
 * `Location` stores a line and a position in that line as the location of some character in a
 * source code.
 *
 * - **field:** `line` - the line within the source
 * - **field:** `pos`  - the position within the line
 */
typedef struct Location {
  unsigned int line;
  unsigned int pos;
} Location;


/**
 * `loc()` returns a location with a given line and position.
 *
 * - **param:** `line` - the location's line
 * - **param:** `pos`  - the location's position
 * - **return:** the location with a given line and position
 */
#define loc(l, p) (Location){ .line=l, .pos=p }


#endif  // __LOC_H__
