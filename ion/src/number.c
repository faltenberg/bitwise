#include "number.h"


Number numFromInt(int value) {
  return (Number){ .value=NULL };
}


Number numFromString(string s) {
  return (Number){ .value=NULL };
}


void deleteNum(Number* num) {
  sbufFree(num->value);
}


int bitSize(Number num) {
  return 0;
}


Number add(Number a, Number b) {
  return (Number){ .value=NULL };
}


Number sub(Number a, Number b) {
  return (Number){ .value=NULL };
}


Number neg(Number a) {
  return (Number){ .value=NULL };
}


string toDecString(Number num) {
  return stringFromArray("");
}


string toHexString(Number num) {
  return stringFromArray("0x");
}


string toBinString(Number num) {
  return stringFromArray("0b");
}
