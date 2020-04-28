CFLAGS=-std=c11 -g -static -fno-common
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

knrcc: $(OBJS)
	$(CC) -o knrcc $(OBJS) $(LDFLAGS)

$(OBJS): knrcc.h

test: knrcc
	./test.sh

clean:
	rm -f knrcc *.o *~ tmp*

.PHONY: test clean