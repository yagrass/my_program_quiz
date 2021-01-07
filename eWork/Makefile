PWD := $(shell pwd)
SRC := BinaryTree wk01_test wk02_test wk03_test wk04_test wk05_test wk06_test wk07_project
INC := include
VPATH = $(SRC) $(INC)
vpath %.c $(SRC)
vpath %.h $(INC)

CC := gcc
CCPP := g++
FLAG := -I$(INC) -c
FLAGG := -I$(INC) -c -g

w75_13perfLAB: w75_13perfLAB.o clock.o fcyc.o
	$(CC) -o $@ w75_13perfLAB.o clock.o fcyc.o
	@rm -f *.o

w75_13perfLAB.o: w75_13perfLAB.c
	@$(CC) $(FLAG) $<

clock.o: clock.c
	@$(CC) $(FLAG) $<

fcyc.o: fcyc.c	
	@$(CC) $(FLAG) $<
		
w75_13perf: w75_13perf.o
	$(CC) -o $@ $<
	@rm -f *.o

w75_13perf.o: w75_13perf.c
	@$(CC) $(FLAGG) $<

w75_13: w75_13.o
	$(CC) -o $@ $<
	@rm -f *.o

w75_13.o: w75_13.c
	@$(CC) $(FLAGG) $<

clocktime: clocktime.o
	$(CC) -o $@ $<
	@rm -f *.o

clocktime.o: clocktime.c
	@$(CC) $(FLAG) $<

513: 513.o
	$(CC) -o $@ $<
	@rm -f *.o

513.o: 513.c
	@$(CC) $(FLAG) $<

cpuid: cpuid.o
	$(CCPP) -o $@ $<
	@rm -f *.o

cpuid.o: cpuid.cpp
	@$(CCPP) $(FLAG) $<

wk07Project: wk07Project.o
	$(CC) -o $@ $<
	@rm -f *.o

wk07Project.o: wk07Project.c
	@$(CC) $(FLAG) $<

wk06Test_4: wk06Test_4.o
	$(CC) -o $@ $<
	@rm -f *.o

wk06Test_4.o: wk06Test_4.c
	@$(CC) $(FLAG) $<

wk06Test_3: wk06Test_3.o
	$(CC) -o $@ $<
	@rm -f *.o

wk06Test_3.o: wk06Test_3.c
	@$(CC) $(FLAG) $<

wk06Test_2: wk06Test_2.o
	$(CC) -o $@ $<
	@rm -f *.o

wk06Test_2.o: wk06Test_2.c
	@$(CC) $(FLAG) $<

wk06Test_1: wk06Test_1.o
	$(CC) -o $@ $<
	@rm -f *.o

wk06Test_1.o: wk06Test_1.c
	@$(CC) $(FLAG) $<

wk05Test_3: wk05Test_3.o
	$(CC) -o $@ $<
	@rm -f *.o

wk05Test_3.o: wk05Test_3.c
	@$(CC) $(FLAG) $<

wk05Test_2: wk05Test_2.o
	$(CC) -o $@ $<
	@rm -f *.o

wk05Test_2.o: wk05Test_2.c
	@$(CC) $(FLAG) $<

wk05Test_1: wk05Test_1.o
	$(CC) -o $@ $<
	@rm -f *.o

wk05Test_1.o: wk05Test_1.c
	@$(CC) $(FLAG) $<

wk04Test_4: wk04Test_4.o
	$(CC) -o $@ $<
	@rm -f *.o

wk04Test_4.o: wk04Test_4.c
	@$(CC) $(FLAG) $<

wk04Test_3: wk04Test_3.o
	$(CC) -o $@ $<
	@rm -f *.o

wk04Test_3.o: wk04Test_3.c
	@$(CC) $(FLAG) $<

wk04Test_2: wk04Test_2.o
	$(CC) -o $@ $<
	@rm -f *.o

wk04Test_2.o: wk04Test_2.c
	@$(CC) $(FLAG) $<

wk04Test_1: wk04Test_1.o
	$(CC) -o $@ $<
	@rm -f *.o

wk04Test_1.o: wk04Test_1.c
	@$(CC) $(FLAG) $<

wk03Test_5: wk03Test_5.o
	$(CC) -o $@ $<
	@rm -f *.o

wk03Test_5.o: wk03Test_5.c
	@$(CC) $(FLAG) $<

wk03Test_4: wk03Test_4.o
	$(CC) -o $@ $<
	@rm -f *.o

wk03Test_4.o: wk03Test_4.c
	@$(CC) $(FLAG) $<

wk03Test_3: wk03Test_3.o
	$(CC) -o $@ $<
	@rm -f *.o

wk03Test_3.o: wk03Test_3.c
	@$(CC) $(FLAG) $<

wk03Test_1: wk03Test_1.o
	$(CC) -o $@ $<
	@rm -f *.o

wk03Test_1.o: wk03Test_1.c
	@$(CC) $(FLAG) $<

wk02Test_6: wk02Test_6.o
	$(CC) -o $@ $<
	@rm -f *.o

wk02Test_6.o: wk02Test_6.c ansWK02Test.h
	@$(CC) $(FLAG) $<

wk02Test_5: wk02Test_5.o
	$(CC) -o $@ $<
	@rm -f *.o

wk02Test_5.o: wk02Test_5.c ansWK02Test.h
	@$(CC) $(FLAG) $<

wk02Test_4: wk02Test_4.o
	$(CC) -o $@ $<
	@rm -f *.o

wk02Test_4.o: wk02Test_4.c ansWK02Test.h
	@$(CC) $(FLAG) $<

wk02Test_3: wk02Test_3.o
	$(CC) -o $@ $<
	@rm -f *.o

wk02Test_3.o: wk02Test_3.c ansWK02Test.h
	@$(CC) $(FLAG) $<

wk02Test_2_e2: wk02Test_2_e2.o
	$(CC) -o $@ $<
	@rm -f *.o

wk02Test_2_e2.o: wk02Test_2_e2.c ansWK02Test.h
	@$(CC) $(FLAG) $<

wk02Test_2: wk02Test_2.o
	$(CC) -o $@ $<
	@rm -f *.o

wk02Test_2.o: wk02Test_2.c ansWK02Test.h
	@$(CC) $(FLAG) $<

wk02Test_1_ASCII_e2: wk02Test_1_ASCII_e2.o
	$(CC) -o $@ $<
	@rm -f *.o

wk02Test_1_ASCII_e2.o: wk02Test_1_ASCII_e2.c 	ansWK02Test.h
	@$(CC) $(FLAG) $<

wk02Test_1_ASCII: wk02Test_1_ASCII.o
	$(CC) -o $@ $<
	@rm -f *.o

wk02Test_1_ASCII.o: wk02Test_1_ASCII.c 	ansWK02Test.h
	@$(CC) $(FLAG) $<

wk01Test_R: wk01Test_R.o
	$(CC) -o $@ $<
	@rm -f *.o

wk01Test_R.o: wk01Test_R.c 	ansWK01Test.h
	@$(CC) $(FLAG) $<

wk01Test_P: wk01Test_P.o
	$(CC) -o $@ $<
	@rm -f *.o

wk01Test_P.o: wk01Test_P.c 	ansWK01Test.h
	@$(CC) $(FLAG) $<

wk01Test: wk01Test.o
	$(CC) -o $@ $<
	@rm -f *.o

wk01Test.o: wk01Test.c 	ansWK01Test.h
	@$(CC) $(FLAG) $<

bTree: bTree.o
	@$(CC) -o $@ $<

bTree.o: bTree.c tree.h
	@$(CC) $(FLAG) $<
	@echo finished @ $(shell date)!

# .PHONY: clean
clean:
	@rm -f *.o
	@rm -f bTree
	@rm -f wk01Test
	@rm -f wk01Test_P
	@rm -f wk01Test_R
	@rm -f wk02Test_1_ASCII
	@rm -f wk02Test_1_ASCII_e2
	@rm -f wk02Test_2
	@rm -f wk02Test_2_e2
	@rm -f wk02Test_3
	@rm -f wk02Test_4
	@rm -f wk02Test_5
	@rm -f wk02Test_6
	@rm -f wk03Test_1
	@rm -f wk03Test_3
	@rm -f wk03Test_4
	@rm -f wk03Test_5
	@rm -f wk04Test_1
	@rm -f wk04Test_2
	@rm -f wk04Test_3
	@rm -f wk04Test_4
	@rm -f wk05Test_1
	@rm -f wk05Test_2
	@rm -f wk05Test_3
	@rm -f wk06Test_1
	@rm -f wk06Test_2
	@rm -f wk06Test_3
	@rm -f wk06Test_4
	@rm -f wk07Project
	@rm -f cpuid
	@rm -f 513
	@rm -f clocktime
	@rm -f w75_13
	@rm -f w75_13perf
	@rm -f w75_13perfLAB