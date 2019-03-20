#include "cunit.h"

#include "arena.h"


#define assertAlignment(ptr, a) __assertAlignment(__FILE__, __LINE__, ptr, a)
bool __assertAlignment(const char* file, int line, const void* pointer, int alignment) {
  printVerbose(__PROMPT, file, line);
  if ((size_t) pointer % alignment == 0) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%p] aligned to [%d]\n", pointer, alignment);
    return false;
  }
}


static TestResult testCreation() {
  TestResult result = {};

  {
    Arena arena = {};
    TEST(assertNull(arena.ptr));
    TEST(assertNull(arena.end));
    TEST(assertNull(arena.blocks));
    TEST(assertEqualSize(arena.totalSpace, 0));
    TEST(assertEqualSize(arena.usedSpace, 0));
    arenaFree(&arena);
  }

  {
    Arena arena = {};
    void* p = arenaAlloc(&arena, 1);
    TEST(assertNotNull(arena.ptr));
    TEST(assertNotNull(arena.end));
    TEST(assertNotNull(arena.blocks));
    TEST(assertEqualSize(arena.totalSpace, 1));
    TEST(assertEqualSize(arena.usedSpace, 1));
    TEST(assertNotNull(p));
    arenaFree(&arena);
  }

  return result;
}


static TestResult testDeletion() {
  TestResult result = {};

  {
    Arena arena = {};
    arenaFree(&arena);
    TEST(assertNull(arena.ptr));
    TEST(assertNull(arena.end));
    TEST(assertNull(arena.blocks));
    TEST(assertEqualSize(arena.totalSpace, 0));
    TEST(assertEqualSize(arena.usedSpace, 0));
  }

  {
    Arena arena = {};
    arenaAlloc(&arena, 1);
    arenaFree(&arena);
    TEST(assertNull(arena.ptr));
    TEST(assertNull(arena.end));
    TEST(assertNull(arena.blocks));
    TEST(assertEqualSize(arena.totalSpace, 0));
    TEST(assertEqualSize(arena.usedSpace, 0));
  }

  return result;
}


static TestResult testAlignment() {
  TestResult result = {};

  {
    Arena arena = {};
    void* p = arenaAlloc(&arena, 1);
    TEST(assertAlignment(p, 1));
    arenaFree(&arena);
  }

  {
    Arena arena = {};
    void* p = arenaAlloc(&arena, 2);
    TEST(assertAlignment(p, 2));
    arenaFree(&arena);
  }

  {
    Arena arena = {};
    void* p = arenaAlloc(&arena, 3);
    TEST(assertAlignment(p, 4));
    arenaFree(&arena);
  }

  {
    Arena arena = {};
    void* p = arenaAlloc(&arena, 4);
    TEST(assertAlignment(p, 4));
    arenaFree(&arena);
  }

  {
    Arena arena = {};
    void* p = arenaAlloc(&arena, 5);
    TEST(assertAlignment(p, 8));
    arenaFree(&arena);
  }

  {
    Arena arena = {};
    void* p = arenaAlloc(&arena, 6);
    TEST(assertAlignment(p, 8));
    arenaFree(&arena);
  }

  {
    Arena arena = {};
    void* p = arenaAlloc(&arena, 7);
    TEST(assertAlignment(p, 8));
    arenaFree(&arena);
  }

  {
    Arena arena = {};
    void* p = arenaAlloc(&arena, 8);
    TEST(assertAlignment(p, 8));
    arenaFree(&arena);
  }

  return result;
}


TestResult arena_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<arena>", "Test arena memory allocator.");
  addTest(&suite, &testCreation,  "testCreation");
  addTest(&suite, &testDeletion,  "testDeletion");
  addTest(&suite, &testAlignment, "testAlignment");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
