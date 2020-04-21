CFLAGS=-std=c11 -g -static

knrcc: knrcc.c

test: knrcc
	./test.sh

clean:
	rm -f knrcc *.o *~ tmp*

.PHONY: test clean