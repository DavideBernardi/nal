CFLAGS = -Wall -Wextra -pedantic -ansi -O2 -lm
SFLAGS = -fsanitize=address -fsanitize=undefined -g3 -lm
DFLAGS = -Wall -Wextra -pedantic -ansi -g3 -lm
CC = clang
BASE = nal
NAME = extension
VARFILE = exvList
FUNCFILE = fMap
BEXECS = exparse exinterp
SEXECS = exparse_s exinterp_s
DEXECS = exparse_d exinterp_d
EXTRACHECKS = $(VARFILE).c $(VARFILE).h $(FUNCFILE).c $(FUNCFILE).h
EXTRAFILES = $(VARFILE).c $(FUNCFILE).c
EXECS = $(BEXECS) $(SEXECS) $(DEXECS)
INTERP = -DINTERP -DINTFILE -DINTABORT -DINTIN2STR -DINTINNUM -DINTJUMP -DINTPRINT -DINTRND -DINTIF -DINTINC -DINTSET -DINTFUNC

all : $(BEXECS)

exparse : $(NAME).c $(NAME).h $(EXTRACHECKS)
		$(CC) $(NAME).c $(EXTRAFILES) $(CFLAGS) -o $@
exparse_s : $(NAME).c $(NAME).h $(EXTRACHECKS)
		$(CC) $(NAME).c $(EXTRAFILES) $(SFLAGS) -o $@
exparse_d : $(NAME).c $(NAME).h $(EXTRACHECKS)
		$(CC) $(NAME).c $(EXTRAFILES) $(DFLAGS) -o $@


exinterp : $(NAME).c $(NAME).h $(EXTRACHECKS)
		$(CC) $(NAME).c $(EXTRAFILES) $(CFLAGS) -o $@ $(INTERP)
exinterp_s : $(NAME).c $(NAME).h $(EXTRACHECKS)
		$(CC) $(NAME).c $(EXTRAFILES) $(SFLAGS) -o $@ $(INTERP)
exinterp_d : $(NAME).c $(NAME).h $(EXTRACHECKS)
		$(CC) $(NAME).c $(EXTRAFILES) $(DFLAGS) -o $@ $(INTERP)


test : testparse testinterp

testparse : testparsecorrect testparseerrors

testinterp: testinterpcorrect testinterperrors

testcorrect: testparsecorrect testinterpcorrect

testerrors : testparseerrors testinterperrors

testparsecorrect : exparse_s exparse_d

	valgrind ./exparse_d extest1.$(BASE)
	valgrind ./exparse_d extest2.$(BASE)
	valgrind ./exparse_d extest3.$(BASE)
	valgrind ./exparse_d extest4.$(BASE)
	valgrind ./exparse_d exescape211.$(BASE)
	valgrind ./exparse_d exlabne.$(BASE)
	valgrind ./exparse_d exmunge.$(BASE)
	./exparse_s extest1.$(BASE)
	./exparse_s extest2.$(BASE)
	./exparse_s extest3.$(BASE)
	./exparse_s extest4.$(BASE)
	./exparse_s exescape211.$(BASE)
	./exparse_s exlabne.$(BASE)
	./exparse_s exmunge.$(BASE)

testparseerrors : exparse_s exparse_d

	-valgrind ./exparse_d extestf1.$(BASE)
	-valgrind ./exparse_d extestf2.$(BASE)
	-valgrind ./exparse_d extestf3.$(BASE)
	-./exparse_s extestf1.$(BASE)
	-./exparse_s extestf2.$(BASE)
	-./exparse_s extestf3.$(BASE)

testinterpcorrect : exinterp_s exinterp_d

	valgrind ./exinterp_d extest1.$(BASE)
	valgrind ./exinterp_d extest2.$(BASE)
	valgrind ./exinterp_d extest3.$(BASE)
	valgrind ./exinterp_d extest4.$(BASE)
	./exinterp_s extest1.$(BASE)
	./exinterp_s extest2.$(BASE)
	./exinterp_s extest3.$(BASE)
	./exinterp_s extest4.$(BASE)

testinterperrors : exinterp_s exinterp_d

	-valgrind ./exinterp_d extestf1.$(BASE)
	-valgrind ./exinterp_d extestf2.$(BASE)
	-valgrind ./exinterp_d extestf3.$(BASE)
	-./exinterp_s extestf1.$(BASE)
	-./exinterp_s extestf2.$(BASE)
	-./exinterp_s extestf3.$(BASE)

clean :
	rm -fr $(EXECS)
