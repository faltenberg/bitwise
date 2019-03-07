OPT_DEBUG = -O0 -g -Wall
OPT_RELEASE = -O3

#OPT = ${OPT_DEBUG}
OPT = ${OPT_RELEASE}


.PHONY: all
all: lib exe test_exe


.PHONY: clean
clean:
	rm -rf bin/


.PHONY: run
run: exe
	@echo ""
	@bin/ion example/demo.ion
	@echo "--------------------------------------------------------------------------------"


.PHONY: test
test: test_exe
	@echo ""
	@bin/test
	@echo "--------------------------------------------------------------------------------"


.PHONY: test_memleak
test_memleak: test_exe
	@echo ""
	@valgrind --leak-check=full bin/test > /dev/null
	@echo "--------------------------------------------------------------------------------"


######################################## INTERNAL ########################################


.PHONY: lib
lib: deps


.PHONY: exe
exe: deps lib
	mkdir -p bin/
	clang ${OPT} -Iinclude/ -Ideps/ -o bin/ion src/*.c


.PHONY: deps
deps:


.PHONY: test_exe
test_exe: lib test_deps
	$(eval SRC_FILES := $(filter-out src/ion.c, $(wildcard src/*.c)) )
	mkdir -p bin/
	clang ${OPT} -Iinclude/ -Ideps/cunit/include/ -Ldeps/cunit/lib/ -o bin/test test/*.c ${SRC_FILES} -lcunit


.PHONY: test_deps
test_deps:
	$(MAKE) -C ./deps/cunit/
