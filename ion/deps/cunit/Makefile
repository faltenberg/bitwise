OPT_DEBUG = -O0 -g -Wall
OPT_RELEASE = -O3

#OPT = ${OPT_DEBUG}
OPT = ${OPT_RELEASE}


.PHONY: all
all: lib exe


.PHONY: run
run: exe
	@echo ""
	@bin/main
	@echo "--------------------------------------------------------------------------------"


.PHONY: lib
lib: deps
	mkdir -p temp/
	mkdir -p lib/
	clang ${OPT} -c -Iinclude/ -o temp/cunit.o src/cunit.c
	ar -rcs lib/libcunit.a temp/cunit.o
	rm -rf temp/


.PHONY: exe
exe: deps lib
	mkdir -p bin/
	clang ${OPT} -Iinclude/ -Llib/ -o bin/main src/*.c -lcunit


.PHONY: test
test: test_deps lib


.PHONY: deps
deps:


.PHONY: test_deps
test_deps:


.PHONY: clean
clean:
	rm -rf temp/
	rm -rf lib/
	rm -rf bin/
