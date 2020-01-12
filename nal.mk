CFLAGS = -Wall -Wextra -pedantic -ansi -O2 -lm
SFLAGS = -fsanitize=address -fsanitize=undefined -g3 -lm
DFLAGS = -Wall -Wextra -pedantic -ansi -g3 -lm
CC = clang
BASE = nal
VARFILE = vList
BEXECS = parse interp
SEXECS = parse_s interp_s
DEXECS = parse_d interp_d
EXTRACHECKS = $(VARFILE).c $(VARFILE).h
EXTRAFILES = $(VARFILE).c
EXECS = $(BEXECS) $(SEXECS) $(DEXECS)

all : $(BEXECS)

parse : $(BASE).c $(EXTRACHECKS)
		$(CC) $(BASE).c $(EXTRAFILES) $(CFLAGS) -o $@
parse_s : $(BASE).c $(EXTRACHECKS)
		$(CC) $(BASE).c $(EXTRAFILES) $(SFLAGS) -o $@
parse_d : $(BASE).c $(EXTRACHECKS)
		$(CC) $(BASE).c $(EXTRAFILES) $(DFLAGS) -o $@


interp : $(BASE).c $(EXTRACHECKS)
		$(CC) $(BASE).c $(EXTRAFILES) $(CFLAGS) -o $@ -DINTERP
interp_s : $(BASE).c $(EXTRACHECKS)
		$(CC) $(BASE).c $(EXTRAFILES) $(SFLAGS) -o $@ -DINTERP
interp_d : $(BASE).c $(EXTRACHECKS)
		$(CC) $(BASE).c $(EXTRAFILES) $(DFLAGS) -o $@ -DINTERP


test : testparse testinterp

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

testinterp : interp_s interp_d
	valgrind ./interp_d t1.$(BASE)
	valgrind ./interp_d t2.$(BASE)
	valgrind ./interp_d t3.$(BASE)
	valgrind ./interp_d t4.$(BASE)
	valgrind ./interp_d t6.$(BASE)
	valgrind ./interp_d t7.$(BASE)
	valgrind ./interp_d t8.$(BASE)
	valgrind ./interp_d t9.$(BASE)
	valgrind ./interp_d t10.$(BASE)
	valgrind ./interp_d t11.$(BASE)
	valgrind ./interp_d t12.$(BASE)
	./interp_s t1.$(BASE)
	./interp_s t2.$(BASE)
	./interp_s t3.$(BASE)
	./interp_s t4.$(BASE)
	./interp_s t6.$(BASE)
	./interp_s t7.$(BASE)
	./interp_s t8.$(BASE)
	./interp_s t9.$(BASE)
	./interp_s t10.$(BASE)
	./interp_s t11.$(BASE)
	./interp_s t12.$(BASE)


clean :
	rm -fr $(EXECS)
