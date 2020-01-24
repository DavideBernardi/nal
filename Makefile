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
INTERP = -DINTERP -DINTFILE -DINTABORT -DINTIN2STR -DINTINNUM -DINTJUMP -DINTPRINT -DINTRND -DINTIF -DINTINC -DINTSET

all : $(BEXECS)

parse : $(BASE).c $(EXTRACHECKS)
		$(CC) $(BASE).c $(EXTRAFILES) $(CFLAGS) -o $@
parse_s : $(BASE).c $(EXTRACHECKS)
		$(CC) $(BASE).c $(EXTRAFILES) $(SFLAGS) -o $@
parse_d : $(BASE).c $(EXTRACHECKS)
		$(CC) $(BASE).c $(EXTRAFILES) $(DFLAGS) -o $@


interp : $(BASE).c $(EXTRACHECKS)
		$(CC) $(BASE).c $(EXTRAFILES) $(CFLAGS) -o $@ $(INTERP)
interp_s : $(BASE).c $(EXTRACHECKS)
		$(CC) $(BASE).c $(EXTRAFILES) $(SFLAGS) -o $@ $(INTERP)
interp_d : $(BASE).c $(EXTRACHECKS)
		$(CC) $(BASE).c $(EXTRAFILES) $(DFLAGS) -o $@ $(INTERP)


test : testparse testinterp

testparse : testparsecorrect testparseerrors

testinterp: testinterpcorrect testinterperrors

testcorrect: testparsecorrect testinterpcorrect

testerrors : testparseerrors testinterperrors

testparsecorrect : parse_s parse_d

	for number in 1 2 3 4 5 6; do \
		valgrind ./parse_d test$$number.$(BASE) ; \
	done
	for number in 1 2 3 4 5 6 7 8 9 10 11 12 13 14; do \
		valgrind ./parse_d t$$number.$(BASE) ; \
	done
	valgrind ./parse_d escape211.$(BASE)
	valgrind ./parse_d labne.$(BASE)
	valgrind ./parse_d labnw.$(BASE)
	valgrind ./parse_d labse.$(BASE)
	valgrind ./parse_d labsw.$(BASE)
	valgrind ./parse_d munge.$(BASE)
	for number in 1 2 3 4 5 6; do \
		./parse_s test$$number.$(BASE) ; \
	done
	for number in 1 2 3 4 5 6 7 8 9 10 11 12 13 14; do \
		./parse_s t$$number.$(BASE) ; \
	done
	./parse_s escape211.$(BASE)
	./parse_s labne.$(BASE)
	./parse_s labnw.$(BASE)
	./parse_s labse.$(BASE)
	./parse_s labsw.$(BASE)
	./parse_s munge.$(BASE)

#The following are supposed to give an error, which is ignored thanks to the '-'
testparseerrors : parse_s parse_d

	-valgrind ./parse_d tf1.$(BASE)
	-valgrind ./parse_d tf2.$(BASE)
	-valgrind ./parse_d tf3.$(BASE)
	-valgrind ./parse_d tf4.$(BASE)
	-valgrind ./parse_d tf4.$(BASE)
	-valgrind ./parse_d tf6.$(BASE)
	-valgrind ./parse_d tf7.$(BASE)
	-valgrind ./parse_d tf8.$(BASE)
	-valgrind ./parse_d tf9.$(BASE)
	-valgrind ./parse_d tf10.$(BASE)
	-valgrind ./parse_d tf11.$(BASE)
	-valgrind ./parse_d tf12.$(BASE)
	-valgrind ./parse_d tf13.$(BASE)
	-valgrind ./parse_d tf14.$(BASE)
	-./parse_s tf1.$(BASE)
	-./parse_s tf2.$(BASE)
	-./parse_s tf3.$(BASE)
	-./parse_s tf4.$(BASE)
	-./parse_s tf4.$(BASE)
	-./parse_s tf6.$(BASE)
	-./parse_s tf7.$(BASE)
	-./parse_s tf8.$(BASE)
	-./parse_s tf9.$(BASE)
	-./parse_s tf10.$(BASE)
	-./parse_s tf11.$(BASE)
	-./parse_s tf12.$(BASE)
	-./parse_s tf13.$(BASE)
	-./parse_s tf14.$(BASE)

testinterpcorrect : interp_s interp_d

	for number in 1 2 4 5; do \
		valgrind ./interp_d test$$number.$(BASE) ; \
	done
	for number in 1 2 3 4 5 6 7 8 9 10 11 12 13 14; do \
		valgrind ./interp_d t$$number.$(BASE) ; \
	done
	for number in 1 2 4 5; do \
		./interp_s test$$number.$(BASE) ; \
	done
	for number in 1 2 3 4 5 6 7 8 9 10 11 12 13 14; do \
		./interp_s t$$number.$(BASE) ; \
	done

#The following are supposed to give an error, which is ignored thanks to the '-'
testinterperrors : interp_s interp_d

	-valgrind ./interp_d tf1.$(BASE)
	-valgrind ./interp_d tf2.$(BASE)
	-valgrind ./interp_d tf3.$(BASE)
	-valgrind ./interp_d tf4.$(BASE)
	-valgrind ./interp_d tf4.$(BASE)
	-valgrind ./interp_d tf6.$(BASE)
	-valgrind ./interp_d tf7.$(BASE)
	-valgrind ./interp_d tf8.$(BASE)
	-valgrind ./interp_d tf9.$(BASE)
	-valgrind ./interp_d tf10.$(BASE)
	-valgrind ./interp_d tf11.$(BASE)
	-valgrind ./interp_d tf12.$(BASE)
	-valgrind ./interp_d tf13.$(BASE)
	-valgrind ./interp_d tf14.$(BASE)
	-./interp_s tf1.$(BASE)
	-./interp_s tf2.$(BASE)
	-./interp_s tf3.$(BASE)
	-./interp_s tf4.$(BASE)
	-./interp_s tf4.$(BASE)
	-./interp_s tf6.$(BASE)
	-./interp_s tf7.$(BASE)
	-./interp_s tf8.$(BASE)
	-./interp_s tf9.$(BASE)
	-./interp_s tf10.$(BASE)
	-./interp_s tf11.$(BASE)
	-./interp_s tf12.$(BASE)
	-./interp_s tf13.$(BASE)
	-./interp_s tf14.$(BASE)

clean :
	rm -fr $(EXECS)
