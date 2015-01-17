LIBS=libpng tslib

BASE_DIR=$(shell pwd)
CFLAGS=-Wall -Wextra -Werror -O3 $(shell pkg-config ${LIBS} --cflags) -I${BASE_DIR}

LDFLAGS=-g $(shell pkg-config ${LIBS} --libs)

SOURCES=${wildcard *.c */*.c}
HEADERS=${wildcard *.h */*.h}
OBJECTS=$(SOURCES:%.c=%.o)
DEPS=$(SOURCES:%.c=%.d)

DEPLOY_HOST?=desco
DEPLOY_DIR?=/root/desco

BIN=desco

all: ${BIN}

# deploy depends on the binary so that we're sure it compiles here, at least.
deploy: ${BIN}
	rsync --exclude='*.o' --exclude='desco' --delete -av * ${DEPLOY_HOST}:${DEPLOY_DIR}
	ssh ${DEPLOY_HOST} make -C ${DEPLOY_DIR}
	ssh ${DEPLOY_HOST} killall ${BIN} &> /dev/null || exit 0

${BIN}: ${OBJECTS}
	${CC} -o $@ $^ ${LDFLAGS}


%.o:%.c
	${CC} -MMD -o $@ -c $< ${CFLAGS}

.PHONY: clean all

clean:
	rm -rf *.o */*.o ${BIN} *.d

-include ${DEPS}
