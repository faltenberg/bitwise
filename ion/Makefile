OPT_DEBUG = -O0 -g -Wall
OPT_RELEASE = -O3

OPT = ${OPT_DEBUG}
#OPT = ${OPT_RELEASE}


.PHONY: all
all: lib exe


.PHONY: run
run: exe
	@echo ""
	@bin/ion
	@echo "--------------------------------------------------------------------------------"


.PHONY: lib
lib: deps


.PHONY: exe
exe: deps lib
	mkdir -p bin/
	clang ${OPT} -Iinclude/ -Ideps/ -o bin/ion src/*.c


.PHONY: test
test: test_deps lib
	$(eval SRC_FILES := $(filter-out src/ion.c, $(wildcard src/*.c)) )
	mkdir -p bin/
	clang ${OPT} -Iinclude/ -Ideps/cunit/include/ -Ldeps/cunit/lib/ -o bin/test test/*.c ${SRC_FILES} -lcunit
	@echo ""
	@bin/test
	@echo "--------------------------------------------------------------------------------"


.PHONY: deps
deps:


.PHONY: test_deps
test_deps:
	$(MAKE) -C ./deps/cunit/


.PHONY: clean
clean:
	rm -rf bin/
