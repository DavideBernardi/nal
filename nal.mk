CFLAGS = -Wall -Wextra -pedantic -ansi -O2 -lm
SFLAGS = -fsanitize=address -fsanitize=undefined -g3 -lm
DFLAGS = -Wall -Wextra -pedantic -ansi -g3 -lm
CC = clang
BASE = nal
BEXECS = parse
SEXECS = parse_s
DEXECS = parse_d
EXECS = $(BEXECS) $(SEXECS) $(DEXECS)

all : $(BEXECS)

parse : $(BASE).c
		$(CC) $(BASE).c $(CFLAGS) -o $@
parse_s : $(BASE).c
		$(CC) $(BASE).c $(SFLAGS) -o $@
parse_d : $(BASE).c
		$(CC) $(BASE).c $(DFLAGS) -o $@

test : testparse

testparse : parse_s parse_d
	valgrind ./parse_d test1.$(BASE)
	valgrind ./parse_d test2.$(BASE)
	valgrind ./parse_d test3.$(BASE)
	valgrind ./parse_d test4.$(BASE)
	valgrind ./parse_d test5.$(BASE)
	valgrind ./parse_d test6.$(BASE)
	./parse_s test1.$(BASE)
	./parse_s test2.$(BASE)
	./parse_s test3.$(BASE)
	./parse_s test4.$(BASE)
	./parse_s test5.$(BASE)
	./parse_s test6.$(BASE)


clean :
	rm -fr $(EXECS)
