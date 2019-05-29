#ifndef __ERROR_H__
#define __ERROR_H__


/**
 * Error
 * =====
 *
 * During the parsing process many errors may appear like illegal characters or missing closing
 * braces. `Error` encapsulates the message and location in the source where the error occured.
 * Errors can cascade and thus each error may have a preceding cause (or NULL). Errors allocate
 * memory for the error message which must be freed!
 *
 *
 * Example
 * -------
 *
 * ```c {.line-numbers}
 * #include "error.h"
 * #include "source.h"
 * #include "str.h"
 * #include <assert.h>
 * #include <stdio.h>
 * #include <stdlib.h>
 *
 * int main() {
 *   Source src = sourceFromString("  123ABC // error");
 *   // generate an error appearing at (1, 6) for "123ABC" stretching from (1, 5) to (1, 8)
 *   Error error = generateError(&src, loc(1, 3), (1, 6), (1, 8),
 *                               "invalid character '%c' for an integer", src.conent.chars[5]);
 *   assert(error.location.line == 1);
 *   assert(error.location.pos == 6);
 *   assert(error.cause == NULL);
 *   assert(cstrequal(error.message.chars ==
 *     "<cstring>:1:6: error: invalid character 'A' for an integer\n  123ABC // error\n  ~~~^~~\n"
 *   );
 *
 *   error = createError(error.location, "an error did occur", &error);
 *   assert(error.location.line == 1);
 *   assert(error.location.pos == 3);
 *   assert(error.cause != NULL);
 *
 *   printf("%.*s", error.message.len, error.message.chars);
 *   deleteError(&error);
 * }
 * ```
 */


#include "str.h"
#include "loc.h"
#include "source.h"


/**
 * `Error` encapsulates error messages and the location and cause of their appearence. The message
 * must be freed as it was allocated in memory.
 *
 * - **field:** `message`  - the error message
 * - **field:** `location` - the location where the error occured
 * - **field:** `cause`    - the (optional) cause of the error
 */
typedef struct Error {
  string        message;
  Location      location;
  struct Error* cause;
} Error;


/**
 * `createError()` returns an error with given data.
 *
 * - **param:** `loc`     - the location where the error occured
 * - **param:** `message` - the error message
 * - **param:** `cause`   - the pointer to the error that caused this error
 * - **return:** an error with the given data
 */
Error createError(Location loc, string message, Error* cause);


/**
 * `generateError()` generates an error with a complex message. It shall simplify generation of
 * typical compiler errors with a similar form:
 *
 * ```txt
 * <file>:<line>:<pos>: error: <error message>
 * <line content from the file>
 *      ~~~~^~~~~
 * ```
 *
 * For the underlining the start and end location of the text fragment within the source must be
 * provided. Probably that will come directly from a token. The `'^'` will appear at the error
 * location. The message will be generated from the format string and additional parameters in a
 * printf-fashion.
 *
 * - **param:** `loc`    - the location where the error occured
 * - **param:** `src`    - the source file where the error occured
 * - **param:** `start`  - the start location of the faulty text fragment
 * - **param:** `end`    - the end location of the faulty text fragment
 * - **param:** `format` - the format string for the error message
 * - **param:** `...`    - the arguments specified by the format string
 * - **return:** the generated error
 */
Error generateError(Location loc, const Source* src, Location start, Location end,
                    const char* format, ...);


/**
 * `deleteError()` deletes an error and preceding causes recursively.
 *
 * - **param:** `error` - the error to be freed
 */
void deleteError(Error* error);


#endif  // __ERROR_H__
