LIBS=libpng tslib

CFLAGS=-Wall -Wextra -Werror -O3 $(shell pkg-config ${LIBS} --cflags) -I

LDFLAGS=-g $(shell pkg-config ${LIBS} --libs)

SOURCES=${wildcard *.c */*.c}
HEADERS=${wildcard *.h */*.h}
OBJECTS=$(SOURCES:%.c=%.o)

BIN=desco

all: ${BIN}

# deploy depends on the binary so that we're sure it compiles here, at least.
deploy: ${BIN}
	make clean
	rsync -av * desco:/root/desco/
	ssh desco make -C /root/desco
	ssh desco killall desco &> /dev/null || exit 0

${BIN}: ${OBJECTS}
	${CC} -o $@ $^ ${LDFLAGS}


%.o:%.c ${HEADERS}
	${CC} -o $@ -c $< ${CFLAGS}

.PHONY: clean all

clean:
	rm -rf *.o */*.o ${BIN}
