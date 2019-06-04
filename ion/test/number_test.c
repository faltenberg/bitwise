#include "cunit.h"

#include "number.h"


static TestResult testCreation() {
  TestResult result = {};

  {
    FAIL("not implemented yet");
  }

  return result;
}


TestResult number_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<number>", "Test numbers.");
  addTest(&suite, &testCreation, "testCreation");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
