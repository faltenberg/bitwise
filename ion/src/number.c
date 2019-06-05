#include "number.h"


Number numFromInt(int value) {
  return (Number){ .value=value };
}


Number numFromString(string s) {
  return (Number){ .value=atoi(s.chars) };
}


void deleteNum(Number* num) {
}


int bitSize(Number num) {
  return 0;
}


Number add(Number a, Number b) {
  return (Number){ .value=0 };
}


Number sub(Number a, Number b) {
  return (Number){ .value=0 };
}


Number neg(Number a) {
  return (Number){ .value=0 };
}


string toDecString(Number num) {
  return stringFromArray("0");
}


string toHexString(Number num) {
  return stringFromArray("0x");
}


string toBinString(Number num) {
  return stringFromArray("0b");
}
