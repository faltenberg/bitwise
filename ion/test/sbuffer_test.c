#include "cunit/cunit.h"
#include "sbuffer.h"

#include <stdlib.h>
#include <time.h>


static TestResult testBufferHeader() {
  TestResult result = {};
  BufHeader h = {};
  TEST(assertEqualSize(h.length, 0));
  TEST(assertEqualSize(h.capacity, 0));
  TEST(assertEqualSize(sizeof(h), 16));
  return result;
}


static TestResult testEmptyBuffer() {
  TestResult result = {};
  int* buffer = NULL;
  ABORT(assertSame(__bufHeader(buffer), (char*) buffer - sizeof(BufHeader)));
  TEST(assertEqualSize(bufLength(buffer), 0));
  TEST(assertEqualSize(bufCapacity(buffer), 0));
  TEST(assertTrue(__bufFits(buffer, 0)));
  TEST(assertFalse(__bufFits(buffer, 1)));
  return result;
}


static TestResult testGrowthOfEmptyBuffer() {
  TestResult result = {};

  {
    int* buffer = NULL;
    buffer = __bufGrow(buffer, 0, sizeof(int));
    ABORT(assertNotNull(buffer));
    ABORT(assertSame(__bufHeader(buffer), (char*) buffer - sizeof(BufHeader)));
    TEST(assertEqualSize(bufLength(buffer), 0));
    TEST(assertEqualSize(bufCapacity(buffer), 1));
    TEST(assertTrue(__bufFits(buffer, 0)));
    TEST(assertTrue(__bufFits(buffer, 1)));
    TEST(assertFalse(__bufFits(buffer, 2)));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    buffer = __bufGrow(buffer, 1, sizeof(int));
    ABORT(assertNotNull(buffer));
    TEST(assertEqualSize(bufLength(buffer), 0));
    TEST(assertEqualSize(bufCapacity(buffer), 1));
    TEST(assertTrue(__bufFits(buffer, 0)));
    TEST(assertTrue(__bufFits(buffer, 1)));
    TEST(assertFalse(__bufFits(buffer, 2)));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    srand(time(0));
    int newCapacity = rand() % 100 + 2;
    buffer = __bufGrow(buffer, newCapacity, sizeof(int));
    ABORT(assertNotNull(buffer));
    TEST(assertEqualSize(bufLength(buffer), 0));
    TEST(assertEqualSize(bufCapacity(buffer), newCapacity));
    TEST(assertTrue(__bufFits(buffer, newCapacity)));
    TEST(assertFalse(__bufFits(buffer, newCapacity + 1)));
    bufFree(buffer);
  }

  return result;
}


static TestResult testGrowthOfNonEmptyBuffer() {
  TestResult result = {};

  {
    int* buffer = NULL;
    buffer = __bufGrow(buffer, 0, sizeof(int));
    buffer = __bufGrow(buffer, 0, sizeof(int));
    TEST(assertEqualSize(bufCapacity(buffer), 2*1 + 1));
    TEST(assertTrue(__bufFits(buffer, 3)));
    TEST(assertFalse(__bufFits(buffer, 4)));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    buffer = __bufGrow(buffer, 0, sizeof(int));
    buffer = __bufGrow(buffer, 1, sizeof(int));
    TEST(assertEqualSize(bufCapacity(buffer), 2*1 + 1));
    TEST(assertTrue(__bufFits(buffer, 3)));
    TEST(assertFalse(__bufFits(buffer, 4)));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    buffer = __bufGrow(buffer, 1, sizeof(int));
    buffer = __bufGrow(buffer, 0, sizeof(int));
    TEST(assertEqualSize(bufCapacity(buffer), 2*1 + 1));
    TEST(assertTrue(__bufFits(buffer, 3)));
    TEST(assertFalse(__bufFits(buffer, 4)));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    buffer = __bufGrow(buffer, 2, sizeof(int));
    buffer = __bufGrow(buffer, 0, sizeof(int));
    TEST(assertEqualSize(bufCapacity(buffer), 2*2 + 1));
    TEST(assertTrue(__bufFits(buffer, 5)));
    TEST(assertFalse(__bufFits(buffer, 6)));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    buffer = __bufGrow(buffer, 2, sizeof(int));
    buffer = __bufGrow(buffer, 10, sizeof(int));
    TEST(assertEqualSize(bufCapacity(buffer), 10));
    TEST(assertTrue(__bufFits(buffer, 10)));
    TEST(assertFalse(__bufFits(buffer, 11)));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    buffer = __bufGrow(buffer, 2, sizeof(int));
    buffer = __bufGrow(buffer, 10, sizeof(int));
    buffer = __bufGrow(buffer, 10, sizeof(int));
    TEST(assertEqualSize(bufCapacity(buffer), 2*10 + 1));
    TEST(assertTrue(__bufFits(buffer, 21)));
    TEST(assertFalse(__bufFits(buffer, 22)));
    bufFree(buffer);
  }

  return result;
}


static TestResult testFitBuffer() {
  TestResult result = {};

  {
    int* buffer = NULL;
    bufFit(buffer, 0);
    ABORT(assertNull(buffer));
    TEST(assertEqualSize(bufCapacity(buffer), 0));
  }

  {
    int* buffer = NULL;
    bufFit(buffer, 1);
    ABORT(assertNotNull(buffer));
    TEST(assertEqualSize(bufCapacity(buffer), 1));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    bufFit(buffer, 1);
    __bufHeader(buffer)->length = bufCapacity(buffer);
    bufFit(buffer, 0);
    TEST(assertEqualSize(bufCapacity(buffer), 1));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    bufFit(buffer, 1);
    __bufHeader(buffer)->length = bufCapacity(buffer);
    bufFit(buffer, 1);
    TEST(assertEqualSize(bufCapacity(buffer), 2*1 + 1));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    bufFit(buffer, 1);
    __bufHeader(buffer)->length = bufCapacity(buffer);
    bufFit(buffer, 1);
    __bufHeader(buffer)->length = bufCapacity(buffer);
    bufFit(buffer, 1);
    TEST(assertEqualSize(bufCapacity(buffer), 2*3 + 1));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    bufFit(buffer, 1);
    bufFit(buffer, 4);
    TEST(assertEqualSize(bufCapacity(buffer), 4));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    bufFit(buffer, 2);
    __bufHeader(buffer)->length = bufCapacity(buffer);
    bufFit(buffer, 2);
    TEST(assertEqualSize(bufCapacity(buffer), 2*2 + 1));
    bufFree(buffer);
  }

  return result;
}


static TestResult testPushElements() {
  TestResult result = {};

  {
    int* buffer = NULL;
    bufPush(buffer, 42);
    ABORT(assertNotNull(buffer));
    TEST(assertEqualSize(bufLength(buffer), 1));
    TEST(assertEqualSize(bufCapacity(buffer), 1));
    TEST(assertEqualInt(buffer[0], 42));
    TEST(assertTrue(__bufFits(buffer, 0)));
    TEST(assertFalse(__bufFits(buffer, 1)));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    bufPush(buffer, 42);
    bufPush(buffer, 43);
    TEST(assertEqualSize(bufLength(buffer), 2));
    TEST(assertEqualSize(bufCapacity(buffer), 2*1 + 1));
    TEST(assertEqualInt(buffer[0], 42));
    TEST(assertEqualInt(buffer[1], 43));
    TEST(assertTrue(__bufFits(buffer, 1)));
    TEST(assertFalse(__bufFits(buffer, 2)));
    bufFree(buffer);
  }

  {
    int* buffer = NULL;
    for (int i = 0; i < 5; i++) {
      bufPush(buffer, 42+i);
    }
    TEST(assertEqualSize(bufLength(buffer), 5));
    TEST(assertEqualSize(bufCapacity(buffer), 2*3 + 1));
    TEST(assertTrue(__bufFits(buffer, 2)));
    TEST(assertFalse(__bufFits(buffer, 3)));
    for (int i = 0; i < bufLength(buffer); i++) {
      TEST(assertEqualInt(buffer[i], 42+i));
    }
    bufFree(buffer);
  }

  return result;
}


static TestResult testFreeBuffer() {
  TestResult result = {};

  {
    int* buffer = NULL;
    bufFree(buffer);
    TEST(assertNull(buffer));
    TEST(assertEqualSize(bufLength(buffer), 0));
    TEST(assertEqualSize(bufCapacity(buffer), 0));
  }

  {
    int* buffer = NULL;
    bufPush(buffer, 42);
    ABORT(assertNotNull(buffer));
    bufFree(buffer);
    TEST(assertNull(buffer));
    TEST(assertEqualSize(bufLength(buffer), 0));
    TEST(assertEqualSize(bufCapacity(buffer), 0));
  }

  return result;
}


TestResult sbuffer_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<sbuffer>", "Test stretchy buffers.");
  addTest(&suite, &testBufferHeader,           "testBufferHeader");
  addTest(&suite, &testEmptyBuffer,            "testEmptyBuffer");
  addTest(&suite, &testGrowthOfEmptyBuffer,    "testGrowthOfEmptyBuffer");
  addTest(&suite, &testGrowthOfNonEmptyBuffer, "testGrowthOfNonEmptyBuffer");
  addTest(&suite, &testFitBuffer,              "testFitBuffer");
  addTest(&suite, &testPushElements,           "testPushElements");
  addTest(&suite, &testFreeBuffer,             "testFreeBuffer");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
