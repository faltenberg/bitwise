#ifndef __ERROR_H__
#define __ERROR_H__


/**
 * Error
 * =====
 *
 * During the parsing process many errors may appear like illegal characters or missing closing
 * braces. `Error` encapsulates the message and location in the source where the error occured.
 * Errors can cascade and thus each error may have a preceding cause (or NULL). Complex messages
 * can be generated with the `generateXXX()` functions. These messages are allocated on the heap
 * and must be freed!
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
 *   string msg = generateError(&src, (1, 6), (1, 3), (1, 8),
 *                              "invalid character '%c' for an integer", src.content.chars[5]);
 *   // try also out generateWarning(), generateHint(), generateNote()
 *   Error error = createError(loc(1, 3), msg, cause)
 *   assert(error.location.line == 1);
 *   assert(error.location.pos == 6);
 *   assert(error.cause == NULL);
 *   assert(cstrequal(error.message.chars ==
 *     "<cstring>:1:6: Error: invalid character 'A' for an integer\n  123ABC // error\n  ~~~^~~\n"
 *   );
 *   printf("%.*s", error.message.len, error.message.chars);
 *
 *   error = createError(error.location, "an error did occur", &error);
 *   assert(error.location.line == 1);
 *   assert(error.location.pos == 3);
 *   assert(error.cause != NULL);
 *   printf("%.*s", error.message.len, error.message.chars);
 *
 *   deleteError(&error);  // deletes causes and allocated messages
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
 * - **field:** `location` - the location where the error occured
 * - **field:** `message`  - the error message
 * - **field:** `cause`    - the (optional) cause of the error
 */
typedef struct Error {
  Location      location;
  string        message;
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
 * `deleteError()` deletes an error and preceding causes recursively.
 *
 * - **param:** `error` - the error to be freed
 */
void deleteError(Error* error);


/**
 * `generateError()` generates an error message of this form:
 * ```txt
 * <file>:<line>:<pos>: Error: <message>
 * <line content from the file>
 *       ~~~^~~~
 * ```
 *
 * For the underlining the start and end location of the text fragment within the source must be
 * provided. Probably that will come directly from a token. The `^` will appear at the provided
 * caret location. The message will be generated from the format string and additional parameters
 * in a printf-fashion.
 *
 * - **param:** `src`    - the source file for the message
 * - **param:** `start`  - the start location of the underlined text fragment
 * - **param:** `caret`  - the location for the highlight
 * - **param:** `end`    - the end location of the underlined text fragment
 * - **param:** `format` - the format string for the message
 * - **param:** `...`    - the arguments specified by the format string
 * - **return:** the generated message
 */
string generateError(const Source* src, Location start, Location caret, Location end,
                     const char* format, ...);


/**
 * `generateNote()` generates a note message of this form:
 * ```txt
 * <file>:<line>:<pos>: Note: <message>
 * <line content from the file>
 *       ~~~^~~~
 * ```
 *
 * For the underlining the start and end location of the text fragment within the source must be
 * provided. Probably that will come directly from a token. The `^` will appear at the provided
 * caret location. The message will be generated from the format string and additional parameters
 * in a printf-fashion.
 *
 * - **param:** `src`    - the source file for the message
 * - **param:** `start`  - the start location of the underlined text fragment
 * - **param:** `caret`  - the location for the highlight
 * - **param:** `end`    - the end location of the underlined text fragment
 * - **param:** `format` - the format string for the message
 * - **param:** `...`    - the arguments specified by the format string
 * - **return:** the generated message
 */
string generateNote(const Source* src, Location start, Location caret, Location end,
                    const char* format, ...);


/**
 * `generateWarning()` generates a warning message of this form:
 * ```txt
 * <file>:<line>:<pos>: Warning: <message>
 * <line content from the file>
 *       ~~~^~~~
 * ```
 *
 * For the underlining the start and end location of the text fragment within the source must be
 * provided. Probably that will come directly from a token. The `^` will appear at the provided
 * caret location. The message will be generated from the format string and additional parameters
 * in a printf-fashion.
 *
 * - **param:** `src`    - the source file for the message
 * - **param:** `start`  - the start location of the underlined text fragment
 * - **param:** `caret`  - the location for the highlight
 * - **param:** `end`    - the end location of the underlined text fragment
 * - **param:** `format` - the format string for the message
 * - **param:** `...`    - the arguments specified by the format string
 * - **return:** the generated message
 */
string generateWarning(const Source* src, Location start, Location caret, Location end,
                       const char* format, ...);

/**
 * `generateHint()` generates a hint message of this form:
 * ```txt
 * <file>:<line>:<pos>: Hint: <message>
 * <line content from the file>
 *       ~~~^~~~
 * ```
 *
 * For the underlining the start and end location of the text fragment within the source must be
 * provided. Probably that will come directly from a token. The `^` will appear at the provided
 * caret location. The message will be generated from the format string and additional parameters
 * in a printf-fashion.
 *
 * - **param:** `src`    - the source file for the message
 * - **param:** `start`  - the start location of the underlined text fragment
 * - **param:** `caret`  - the location for the highlight
 * - **param:** `end`    - the end location of the underlined text fragment
 * - **param:** `format` - the format string for the message
 * - **param:** `...`    - the arguments specified by the format string
 * - **return:** the generated message
 */
string generateHint(const Source* src, Location start, Location caret, Location end,
                    const char* format, ...);


#endif  // __ERROR_H__
