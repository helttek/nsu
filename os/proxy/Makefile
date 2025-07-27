TARGET_1 = proxy
SRCS_1 = main.c logger.c picohttpparser.c proxy.c thread_pool.c
INCL_1 = logger.h picohttpparser.h proxy.h thread_pool.h
CC = gcc
RM = rm
CFLAGS = -O2 -Wall
LIBS = -lpthread -lrt
INCLUDE_DIR = "."

all: ${TARGET_1}

${TARGET_1}: ${INCL_1} ${SRCS_1}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_1} ${LIBS} -o ${TARGET_1}

clean:
	${RM} -f *.o ${TARGET_1}
