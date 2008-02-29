# Makefile for ~collin/devel/support/vlist
OUT:=list-test test-alphanumeric 

CFLAGS:=-ffast-math -finline-functions -ggdb3

all: $(OUT)

rsztst: rsztst.o vlist.o

list-test: list-test.o vlist.o

test-alphanumeric: test-alphanumeric.o vlist.o

clean:
	rm -f *.o $(OUT)