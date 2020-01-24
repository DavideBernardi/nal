CFLAGS = -Wall -Wextra -pedantic -ansi -O2 -lm
SFLAGS = -fsanitize=address -fsanitize=undefined -g3 -lm
DFLAGS = -Wall -Wextra -pedantic -ansi -g3 -lm
CC = clang
BASE = nal
NAME = extension
VARFILE = vList
FUNCFILE = fMap
BEXECS = exparse exinterp
SEXECS = exparse_s exinterp_s
DEXECS = exparse_d exinterp_d
EXTRACHECKS = $(VARFILE).c $(VARFILE).h $(FUNCFILE).c $(FUNCFILE).h
EXTRAFILES = $(VARFILE).c $(FUNCFILE).c
EXECS = $(BEXECS) $(SEXECS) $(DEXECS)
INTERP = -DINTERP -DINTFILE -DINTABORT -DINTIN2STR -DINTINNUM -DINTJUMP -DINTPRINT -DINTRND -DINTIF -DINTINC -DINTSET

all : $(BEXECS)

exparse : $(NAME).c $(EXTRACHECKS)
		$(CC) $(NAME).c $(EXTRAFILES) $(CFLAGS) -o $@
exparse_s : $(NAME).c $(EXTRACHECKS)
		$(CC) $(NAME).c $(EXTRAFILES) $(SFLAGS) -o $@
exparse_d : $(NAME).c $(EXTRACHECKS)
		$(CC) $(NAME).c $(EXTRAFILES) $(DFLAGS) -o $@


exinterp : $(NAME).c $(EXTRACHECKS)
		$(CC) $(NAME).c $(EXTRAFILES) $(CFLAGS) -o $@ $(INTERP)
exinterp_s : $(NAME).c $(EXTRACHECKS)
		$(CC) $(NAME).c $(EXTRAFILES) $(SFLAGS) -o $@ $(INTERP)
exinterp_d : $(NAME).c $(EXTRACHECKS)
		$(CC) $(NAME).c $(EXTRAFILES) $(DFLAGS) -o $@ $(INTERP)


test : testparse testinterp

testparse : testparsecorrect testparseerrors

testinterp: testinterpcorrect testinterperrors

testcorrect: testparsecorrect testinterpcorrect

testerrors : testparseerrors testinterperrors

testparsecorrect : exparse_s exparse_d


testparseerrors : exparse_s exparse_d


testinterpcorrect : exinterp_s exinterp_d


testinterperrors : exinterp_s exinterp_d


clean :
	rm -fr $(EXECS)
