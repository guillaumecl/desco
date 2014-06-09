CFLAGS=-Wall -Wextra -Werror -O3
CFLAGS+=$(shell sdl-config --cflags)
LDFLAGS=-g
LDFLAGS+=$(shell sdl-config --libs)

SOURCES=${wildcard *.c}
HEADERS=${wildcard *.h}
OBJECTS=$(SOURCES:%.c=%.o)

BIN=desco

all: ${BIN}

${BIN}: ${OBJECTS}
	${CC} -o $@ $^ ${LDFLAGS}


%.o:%.c ${HEADERS}
	${CC} -o $@ -c $< ${CFLAGS}

.PHONY: clean all

clean:
	rm -rf *.o ${BIN}
