OPT_DEBUG = -O0 -g -Wall
OPT_RELEASE = -O3

OPT = ${OPT_DEBUG}
#OPT = ${OPT_RELEASE}


.PHONY: all
all: lib exe


.PHONY: run
run: exe
	@echo "--------------------------------------------------------------------------------"
	@bin/ion


.PHONY: lib
lib:


.PHONY: exe
exe: lib
	mkdir -p bin/
	clang ${OPT} -Iinclude/ -Ideps/ -o bin/ion src/*.c


.PHONY: test
test: lib
	$(eval SRC_FILES := $(filter-out src/ion.c, $(wildcard src/*.c)) )
	mkdir -p bin/
	clang ${OPT} -Iinclude/ -Ideps/ -Ldeps/cunit/ -o bin/test test/*.c ${SRC_FILES} -lcunit
	@echo "--------------------------------------------------------------------------------"
	@bin/test


.PHONY: clean
clean:
	rm -rf bin/
