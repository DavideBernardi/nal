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
	valgrind ./parse_d t1.$(BASE)
	valgrind ./parse_d t2.$(BASE)
	valgrind ./parse_d t3.$(BASE)
	valgrind ./parse_d t4.$(BASE)
	valgrind ./parse_d t5.$(BASE)
	valgrind ./parse_d t6.$(BASE)
	valgrind ./parse_d t7.$(BASE)
	valgrind ./parse_d t8.$(BASE)
	valgrind ./parse_d t9.$(BASE)
	valgrind ./parse_d escape211.$(BASE)
	valgrind ./parse_d labne.$(BASE)
	valgrind ./parse_d labnw.$(BASE)
	valgrind ./parse_d labse.$(BASE)
	valgrind ./parse_d labsw.$(BASE)
	valgrind ./parse_d munge.$(BASE)
	./parse_s test1.$(BASE)
	./parse_s test2.$(BASE)
	./parse_s test3.$(BASE)
	./parse_s test4.$(BASE)
	./parse_s test5.$(BASE)
	./parse_s test6.$(BASE)
	./parse_s t1.$(BASE)
	./parse_s t2.$(BASE)
	./parse_s t3.$(BASE)
	./parse_s t4.$(BASE)
	./parse_s t5.$(BASE)
	./parse_s t6.$(BASE)
	./parse_s t7.$(BASE)
	./parse_s t8.$(BASE)
	./parse_s t9.$(BASE)
	./parse_s escape211.$(BASE)
	./parse_s labne.$(BASE)
	./parse_s labnw.$(BASE)
	./parse_s labse.$(BASE)
	./parse_s labsw.$(BASE)
	./parse_s munge.$(BASE)



clean :
	rm -fr $(EXECS)
