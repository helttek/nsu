TARGET = sig_hand
SRCS = main.c

CC = gcc
RM = rm
CFLAGS = -O3
LIBS = -lpthread
INCLUDE_DIR = "."

all: ${TARGET}

${TARGET}: ${SRCS}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS} ${LIBS} -o ${TARGET}

clean:
	${RM} -f *.o ${TARGET}
