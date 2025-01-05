TARGET = join_detach
SRCS = main.c

CC=gcc
RM=rm
CFLAGS= -std=c11 -g #-Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET}

${TARGET}: ${SRCS}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS} ${LIBS} -o ${TARGET}

clean:
	${RM} -f *.o ${TARGET}
