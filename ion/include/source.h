#ifndef __SOURCE_H__
#define __SOURCE_H__


/**
 * Sources
 * =======
 *
 * A `Source` encapsulates a source code of a program. A code is some string of characters being
 * origninated from a C string (e.g. for debugging purposes) or a file. A `Source` has a file name
 * and its content. A status flag indicates whether the content was read properly. In case of
 * errors the conent contains an error message. If no errors occured the content is a copy of the
 * source string or file. A `Source` must be released to free the memory, in which case the name
 * and content will be replaced with an empty string.
 *
 *
 * Example
 * -------
 *
 * ```c {.line-numbers}
 * #include "source.h"
 * #include <assert.h>
 * #include "str.h"
 *
 * int main() {
 *   Source src = sourceFromString("foo bar");  // create a copy of a C string
 *   assert(src.status == SOURCE_OK);
 *   assert(cstrequal(src.fileName, "<cstring>"));
 *   assert(cstrequal(src.content, "foo bar"));
 *
 *   deleteSource(&src);  // delete the source
 *   assert(src.status == SOURCE_NONE);
 *   assert(cstrequal(src.fileName, ""));
 *   assert(cstrequal(src.content, ""));
 *
 *   // create lorem.txt
 *   src = sourceFromFile("lorem.txt");  // read the content of the file
 *   assert(src.status == SOURCE_OK);
 *   assert(cstrequal(src.fileName, "lorem.txt"));
 *   assert(cstrequal(src.content, "lorem ipsum dolor"));
 *   deleteSource(&src);
 *
 *   src = sourceFromFile("deleted.txt");  // a file that cannot be read properly
 *   assert(src.status == SOURCE_ERROR);
 *   assert(cstrequal(src.fileName, "deleted.txt"));
 *   assert(cstrequal(src.content, "ERROR: could not open the file"));
 *   deleteSource(&src);
 *
 *   // access distinct lines
 *   src = sourceFromString("line one\nline two");
 *   assert(cstrequal(getLine(&src, 0), ""));
 *   assert(cstrequal(getLine(&src, 1), "line one\n"));
 *   assert(cstrequal(getLine(&src, 2), "line two"));
 *   assert(cstrequal(getLine(&src, 3), ""));
 *   deleteSource(&src);
 * }
 * ```
 */


#include "str.h"
#include "sbuffer.h"


/**
 * `SourceStatus` indicated whether a `Source` was read properly or errors did occur or the data
 * was already released.
 *
 * - **enum:** `SOURCE_OK`    - no errors and source is valid
 * - **enum:** `SOURCE_ERROR` - source wasn't read due to some error
 * - **enum:** `SOURCE_NONE`  - source was freed and contains no data
 */
typedef enum SourceStatus {
  SOURCE_OK,
  SOURCE_ERROR,
  SOURCE_NONE,
} SourceStatus;


/**
 * `Source` contains the source code from a file or C string. If errors did occur the content
 * contains an error message.
 *
 * - **field:** `fileName` - the file name of the source
 * - **field:** `content`  - the content of the source
 * - **field:** `status*   - the status indicating errors
 */
typedef struct Source {
  string       fileName;
  string       content;
  SourceStatus status;
} Source;


/**
 * `sourcefromString()` creates a `Source` as a copy of a C string.
 *
 * - **param:** `src` - a C string containing some source code
 * - **return:** the copied source
 */
Source sourceFromString(const char* src);


/**
 * `sourceFromFile()` reads a file and returns its contents. In case of errors the `status` is set
 * to `SOURCE_ERROR` and `content` will contain an error message.
 *
 * - **param:** `name` - the file name
 * - **return:** the content of the source file
 */
Source sourceFromFile(const char* name);


/**
 * `deleteSource()` deletes the contents of a `Source` and replaces the content with an empty
 * string. `status` is set to `SOURCE_NONE` to indicate that it should not be used anymore.
 *
 * - **param:** `source` - the pointer to the source to be deleted
 */
void deleteSource(Source* source);


/**
 * `getLine()` returns a line of text given by some index. Line counting starts at 1. If the index
 * is zero or greater than the number of lines in the text, an empty string is returned. If the
 * ends with an end-of-line character(s), those characters are included. The returned string is not
 * null-terminated.
 *
 * - **param:** `source` - a pointer to the source text
 * - **param:** `line`   - the line's index
 * - **return:** the string containing the line
 */
string getLine(const Source* source, size_t line);


#endif  // __SOURCE_H__
