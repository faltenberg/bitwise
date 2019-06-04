#ifndef __NUMBER_H__
#define __NUMBER_H__


/**
 *
 */


#include "sbuffer.h"
#include "str.h"


typedef struct Number {
  SBUF(char) value;
} Number;


Number numFromInt(int value);


Number numFromString(string s);


void deleteNum(Number* num);


int bitSize(Number num);


Number add(Number a, Number b);


Number sub(Number a, Number b);


Number neg(Number a);


string toDecString(Number num);


string toHexString(Number num);


string toBinString(Number num);


#endif  // __NUMBER_H__
