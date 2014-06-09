CFLAGS=-Wall -Wextra -Werror -O3
CFLAGS+=$(shell sdl-config --cflags)
LDFLAGS=-g
LDFLAGS+=$(shell sdl-config --libs) -lSDL_image -lSDL_ttf

SOURCES=${wildcard *.c}
HEADERS=${wildcard *.h}
OBJECTS=$(SOURCES:%.c=%.o)

BIN=desco

all: ${BIN}

# deploy depends on the binary so that we're sure it compiles here, at least.
deploy: ${BIN}
	make clean
	rsync -av * desco:/root/desco/
	ssh desco make -C /root/desco

${BIN}: ${OBJECTS}
	${CC} -o $@ $^ ${LDFLAGS}


%.o:%.c ${HEADERS}
	${CC} -o $@ -c $< ${CFLAGS}

.PHONY: clean all

clean:
	rm -rf *.o ${BIN}
