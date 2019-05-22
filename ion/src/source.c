#include "source.h"

#include <string.h>
#include <stdio.h>


Source sourceFromString(const char* src) {
  return (Source){ .fileName=stringFromArray(strdup("<cstring>")),
                   .content=stringFromArray(strdup(src)),
                   .status=SOURCE_OK
                 };
}


/**
 * Opens the file and calculates the file size. Then allocates this amount of memory and reads in
 * the file content. If errors occured stores an error message.
 */
Source sourceFromFile(const char* fileName) {
  char* name = strdup(fileName);
  char* content;
  SourceStatus status = SOURCE_OK;

  FILE* file = fopen(fileName, "r");
  if (file == NULL) {
    status = SOURCE_ERROR;
    content = strdup("ERROR: could not open the file");
    goto EXIT;
  }

  // get file size by putting the cursor to file end (must rewind afterwards)
  if (fseek(file, 0, SEEK_END) != 0) {
    status = SOURCE_ERROR;
    content = strdup("ERROR: could not calculate the file size");
    fclose(file);
    goto EXIT;
  }
  long size = ftell(file);  // current position is the file size
  if (size < 0) {
    status = SOURCE_ERROR;
    content = strdup("ERROR: could not calculate the file size");
    fclose(file);
    goto EXIT;
  }
  rewind(file);  // set cursor back to beginning

  content = (char*) malloc(size+1);
  if (content == NULL) {
    status = SOURCE_ERROR;
    content = strdup("ERROR: could not allocate enough memory");
    fclose(file);
    goto EXIT;
  }

  if (fread(content, 1, size, file) != size) {
    status = SOURCE_ERROR;
    free(content);
    content = strdup("ERROR: could not read all the file content");
    fclose(file);
    goto EXIT;
  } else {
    fclose(file);
    content[size] = '\0';
  }

  EXIT:
  return (Source){ .fileName=stringFromArray(name), .content=stringFromArray(content),
                   .status=status };
}


void deleteSource(Source* source) {
  free((char*)source->fileName.chars);
  free((char*)source->content.chars);
  source->fileName = stringFromArray("");
  source->content = stringFromArray("");
  source->status = SOURCE_NONE;
}


string getLine(const Source* source, size_t line) {
  if (line == 0) {
    return stringFromArray("");
  }

  size_t currentPos, currentLine;

  // skip to line
  for (currentPos = 0, currentLine = 1;
       currentPos < source->content.len && currentLine != line;
       currentPos++) {
    if (source->content.chars[currentPos] == '\n') {
      ++currentLine;
    }
  }

  // if there are too few lines
  if (line < currentLine) {
    return stringFromArray("");
  }

  // read out the current line
  size_t start = currentPos;
  for (; currentPos < source->content.len; currentPos++) {
    if (source->content.chars[currentPos] == '\n') {
      ++currentPos;  // include new line character
      break;
    }
  }

  return stringFromRange(source->content.chars + start, source->content.chars + currentPos);
}
