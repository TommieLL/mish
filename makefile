# Makefile
# Tommie Lindberg
# c15tlg
# Ou3 (mish)
#

CFLAGS += -Wall -Wextra -pedantic -g -std=c99
CC = gcc
OBJS = execute.o parser.o sighant.o

all: ${OBJS}
	${CC} ${CFLAGS} mish.c ${OBJS} -o mish

parser.o: parser.c parser.h

sighant.o: sighant.c sighant.h

execute.o: execute.c execute.h

clean:
	@rm -rf mish *.o *.dSYM

.PHONY: all clean

