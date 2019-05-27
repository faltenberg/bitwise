CC = clang
OPT_DEBUG   = -O0 -g -Wall -Wno-unused-function
OPT_RELEASE = -O3

OPT = ${OPT_DEBUG}
#OPT = ${OPT_RELEASE}


.PHONY: all
all: lib exe


.PHONY: clean
clean:
	rm -rf bin/


.PHONY: reset
reset: clean
	$(MAKE) -C $(wildcard deps/*/) clean


.PHONY: run
run: bin/ion
	@echo ""
	@$< example/demo.ion
	@echo "--------------------------------------------------------------------------------"


.PHONY: print_sizes
print_sizes: bin/print_sizes
	@echo ""
	@$<
	@echo "--------------------------------------------------------------------------------"


.PHONY: test
test: bin/test
	@echo ""
	@$<
	@echo "--------------------------------------------------------------------------------"


.PHONY: memleak
memleak: bin/test
	@echo ""
	@valgrind --leak-check=full $< > /dev/null
	@echo "--------------------------------------------------------------------------------"


######################################## INTERNAL ########################################


EXE_FILES = src/ion.c src/print_sizes.c
SRC_FILES = $(filter-out ${EXE_FILES}, $(wildcard src/*.c))
INC_ARGS  = $(foreach d, include/ $(wildcard deps/*/include/), -I$d)
LINK_ARGS = $(foreach d, lib/ $(wildcard deps/*/lib/), -L$d)
LIBS      = $(foreach x, ${LIBNAMES}, -l$x)


bin/ion: lib
	$(eval LIBNAMES := )
	mkdir -p bin/
	$(CC) ${OPT} ${INC_ARGS} ${LINK_ARGS} -o $@ ${SRC_FILES} src/ion.c ${LIBS}

bin/print_sizes: lib
	$(eval LIBNAMES := )
	mkdir -p bin/
	$(CC) ${OPT} ${INC_ARGS} ${LINK_ARGS} -o $@ ${SRC_FILES} src/print_sizes.c ${LIBS}

bin/test: lib deps_cunit
	$(eval LIBNAMES := cunit)
	mkdir -p bin/
	$(CC) ${OPT} ${INC_ARGS} ${LINK_ARGS} -o $@ ${SRC_FILES} test/*.c ${LIBS}


.PHONY: exe
exe: bin/print_sizes bin/ion


.PHONY: lib
lib:


.PHONY: deps_cunit
deps_cunit:
	$(MAKE) -C $(wildcard deps/cunit/)
