#include "cunit.h"

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
  SBUF(int) buffer = NULL;
  ABORT(assertSame(__sbufHeader(buffer), (char*) buffer - sizeof(BufHeader)));
  TEST(assertEqualSize(sbufLength(buffer), 0));
  TEST(assertEqualSize(sbufCapacity(buffer), 0));
  TEST(assertTrue(__sbufFits(buffer, 0)));
  TEST(assertFalse(__sbufFits(buffer, 1)));
  return result;
}


static TestResult testGrowthOfEmptyBuffer() {
  TestResult result = {};

  {
    SBUF(int) buffer = NULL;
    buffer = __sbufGrow(buffer, 0, sizeof(int));
    ABORT(assertNotNull(buffer));
    ABORT(assertSame(__sbufHeader(buffer), (char*) buffer - sizeof(BufHeader)));
    TEST(assertEqualSize(sbufLength(buffer), 0));
    TEST(assertEqualSize(sbufCapacity(buffer), 1));
    TEST(assertTrue(__sbufFits(buffer, 0)));
    TEST(assertTrue(__sbufFits(buffer, 1)));
    TEST(assertFalse(__sbufFits(buffer, 2)));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    buffer = __sbufGrow(buffer, 1, sizeof(int));
    ABORT(assertNotNull(buffer));
    TEST(assertEqualSize(sbufLength(buffer), 0));
    TEST(assertEqualSize(sbufCapacity(buffer), 1));
    TEST(assertTrue(__sbufFits(buffer, 0)));
    TEST(assertTrue(__sbufFits(buffer, 1)));
    TEST(assertFalse(__sbufFits(buffer, 2)));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    srand(time(0));
    int newCapacity = rand() % 100 + 2;
    buffer = __sbufGrow(buffer, newCapacity, sizeof(int));
    ABORT(assertNotNull(buffer));
    TEST(assertEqualSize(sbufLength(buffer), 0));
    TEST(assertEqualSize(sbufCapacity(buffer), newCapacity));
    TEST(assertTrue(__sbufFits(buffer, newCapacity)));
    TEST(assertFalse(__sbufFits(buffer, newCapacity + 1)));
    sbufFree(buffer);
  }

  return result;
}


static TestResult testGrowthOfNonEmptyBuffer() {
  TestResult result = {};

  {
    SBUF(int) buffer = NULL;
    buffer = __sbufGrow(buffer, 0, sizeof(int));
    buffer = __sbufGrow(buffer, 0, sizeof(int));
    TEST(assertEqualSize(sbufCapacity(buffer), 2*1 + 1));
    TEST(assertTrue(__sbufFits(buffer, 3)));
    TEST(assertFalse(__sbufFits(buffer, 4)));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    buffer = __sbufGrow(buffer, 0, sizeof(int));
    buffer = __sbufGrow(buffer, 1, sizeof(int));
    TEST(assertEqualSize(sbufCapacity(buffer), 2*1 + 1));
    TEST(assertTrue(__sbufFits(buffer, 3)));
    TEST(assertFalse(__sbufFits(buffer, 4)));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    buffer = __sbufGrow(buffer, 1, sizeof(int));
    buffer = __sbufGrow(buffer, 0, sizeof(int));
    TEST(assertEqualSize(sbufCapacity(buffer), 2*1 + 1));
    TEST(assertTrue(__sbufFits(buffer, 3)));
    TEST(assertFalse(__sbufFits(buffer, 4)));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    buffer = __sbufGrow(buffer, 2, sizeof(int));
    buffer = __sbufGrow(buffer, 0, sizeof(int));
    TEST(assertEqualSize(sbufCapacity(buffer), 2*2 + 1));
    TEST(assertTrue(__sbufFits(buffer, 5)));
    TEST(assertFalse(__sbufFits(buffer, 6)));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    buffer = __sbufGrow(buffer, 2, sizeof(int));
    buffer = __sbufGrow(buffer, 10, sizeof(int));
    TEST(assertEqualSize(sbufCapacity(buffer), 10));
    TEST(assertTrue(__sbufFits(buffer, 10)));
    TEST(assertFalse(__sbufFits(buffer, 11)));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    buffer = __sbufGrow(buffer, 2, sizeof(int));
    buffer = __sbufGrow(buffer, 10, sizeof(int));
    buffer = __sbufGrow(buffer, 10, sizeof(int));
    TEST(assertEqualSize(sbufCapacity(buffer), 2*10 + 1));
    TEST(assertTrue(__sbufFits(buffer, 21)));
    TEST(assertFalse(__sbufFits(buffer, 22)));
    sbufFree(buffer);
  }

  return result;
}


static TestResult testFitBuffer() {
  TestResult result = {};

  {
    SBUF(int) buffer = NULL;
    sbufFit(buffer, 0);
    ABORT(assertNull(buffer));
    TEST(assertEqualSize(sbufCapacity(buffer), 0));
  }

  {
    SBUF(int) buffer = NULL;
    sbufFit(buffer, 1);
    ABORT(assertNotNull(buffer));
    TEST(assertEqualSize(sbufCapacity(buffer), 1));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    sbufFit(buffer, 1);
    __sbufHeader(buffer)->length = sbufCapacity(buffer);
    sbufFit(buffer, 0);
    TEST(assertEqualSize(sbufCapacity(buffer), 1));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    sbufFit(buffer, 1);
    __sbufHeader(buffer)->length = sbufCapacity(buffer);
    sbufFit(buffer, 1);
    TEST(assertEqualSize(sbufCapacity(buffer), 2*1 + 1));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    sbufFit(buffer, 1);
    __sbufHeader(buffer)->length = sbufCapacity(buffer);
    sbufFit(buffer, 1);
    __sbufHeader(buffer)->length = sbufCapacity(buffer);
    sbufFit(buffer, 1);
    TEST(assertEqualSize(sbufCapacity(buffer), 2*3 + 1));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    sbufFit(buffer, 1);
    sbufFit(buffer, 4);
    TEST(assertEqualSize(sbufCapacity(buffer), 4));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    sbufFit(buffer, 2);
    __sbufHeader(buffer)->length = sbufCapacity(buffer);
    sbufFit(buffer, 2);
    TEST(assertEqualSize(sbufCapacity(buffer), 2*2 + 1));
    sbufFree(buffer);
  }

  return result;
}


static TestResult testPushElements() {
  TestResult result = {};

  {
    SBUF(int) buffer = NULL;
    sbufPush(buffer, 42);
    ABORT(assertNotNull(buffer));
    TEST(assertEqualSize(sbufLength(buffer), 1));
    TEST(assertEqualSize(sbufCapacity(buffer), 1));
    TEST(assertEqualInt(buffer[0], 42));
    TEST(assertTrue(__sbufFits(buffer, 0)));
    TEST(assertFalse(__sbufFits(buffer, 1)));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    sbufPush(buffer, 42);
    sbufPush(buffer, 43);
    TEST(assertEqualSize(sbufLength(buffer), 2));
    TEST(assertEqualSize(sbufCapacity(buffer), 2*1 + 1));
    TEST(assertEqualInt(buffer[0], 42));
    TEST(assertEqualInt(buffer[1], 43));
    TEST(assertTrue(__sbufFits(buffer, 1)));
    TEST(assertFalse(__sbufFits(buffer, 2)));
    sbufFree(buffer);
  }

  {
    size_t size = 20;
    SBUF(int) buffer = NULL;
    for (int i = 0; i < size; i++) {
      sbufPush(buffer, i);
    }
    TEST(assertEqualSize(sbufLength(buffer), size));
    TEST(assertEqualInt(sbufCapacity(buffer), 2*(2*(2*(2*1+1)+1)+1)+1));
    TEST(assertTrue(__sbufFits(buffer, 11)));
    TEST(assertFalse(__sbufFits(buffer, 12)));
    for (size_t i = 0; i < sbufLength(buffer); i++) {
      TEST(assertEqualInt(buffer[i], i));
    }
    sbufFree(buffer);
  }

  return result;
}


static TestResult testIteration() {
  TestResult result = {};

  {
    SBUF(int) buffer = NULL;
    TEST(assertNull(sbufEnd(buffer)));
  }

  {
    SBUF(int) buffer = NULL;
    sbufPush(buffer, 42);
    ABORT(assertNotNull(sbufEnd(buffer)));
    TEST(assertSame(sbufEnd(buffer), buffer+1));
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    sbufFit(buffer, 10);
    TEST(assertSame(sbufEnd(buffer), buffer));
    __sbufHeader(buffer)->length = sbufCapacity(buffer);
    TEST(assertSame(sbufEnd(buffer), buffer+10))
    sbufFree(buffer);
  }

  {
    SBUF(int) buffer = NULL;
    for (int i = 0; i < 4; i++) {
      sbufPush(buffer, 42+i);
    }
    for (int* it = buffer; it != sbufEnd(buffer); it++) {
      TEST(assertEqualInt(*it, 42 + (it - buffer)));
    }
    sbufFree(buffer);
  }

  {
    SBUF(char*) buffer = NULL;
    sbufPush(buffer, "foo");
    sbufPush(buffer, "bar");
    sbufPush(buffer, "baz");
    const char* expected[] = { "foo", "bar", "baz" };

    int pos = 0;
    for (char** it = buffer; it != sbufEnd(buffer); it++) {
      ABORT(assertFalse(pos >= 3));
      TEST(assertEqualString(*it, expected[pos++]));
    }
    sbufFree(buffer);
  }

  return result;
}


static TestResult testFreeBuffer() {
  TestResult result = {};

  {
    SBUF(int) buffer = NULL;
    sbufFree(buffer);
    TEST(assertNull(buffer));
    TEST(assertEqualSize(sbufLength(buffer), 0));
    TEST(assertEqualSize(sbufCapacity(buffer), 0));
  }

  {
    SBUF(int) buffer = NULL;
    sbufPush(buffer, 42);
    ABORT(assertNotNull(buffer));
    sbufFree(buffer);
    TEST(assertNull(buffer));
    TEST(assertEqualSize(sbufLength(buffer), 0));
    TEST(assertEqualSize(sbufCapacity(buffer), 0));
  }

  return result;
}


TestResult sbuffer_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<sbuffer>", "Test stretchy buffers.");
  addTest(&suite, testBufferHeader);
  addTest(&suite, testEmptyBuffer);
  addTest(&suite, testGrowthOfEmptyBuffer);
  addTest(&suite, testGrowthOfNonEmptyBuffer);
  addTest(&suite, testFitBuffer);
  addTest(&suite, testPushElements);
  addTest(&suite, testIteration);
  addTest(&suite, testFreeBuffer);
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
