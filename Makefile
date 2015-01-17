LIBS=libpng tslib

BASE_DIR=$(shell pwd)
CFLAGS=-Wall -Wextra -Werror -O3 $(shell pkg-config ${LIBS} --cflags) -I${BASE_DIR}
BUILD_DIR=build

LDFLAGS=-g $(shell pkg-config ${LIBS} --libs)

SOURCES=${wildcard *.c */*.c}
HEADERS=${wildcard *.h */*.h}
OBJECTS=$(SOURCES:%.c=${BUILD_DIR}/%.o)
DEPS=$(SOURCES:%.c=${BUILD_DIR}/%.d)
BUILD_DEP=${BUILD_DIR}/phony

DEPLOY_HOST?=desco
DEPLOY_DIR?=/root/desco

BIN=${BUILD_DIR}/desco

all: ${BIN}

# Create all the file hierarchy of folders in the build directory
${BUILD_DEP}:
	find -not -wholename '*/.git*' -not -wholename '*/${BUILD_DIR}*' -type d -exec mkdir -p ${BUILD_DIR}/{} \;
	touch ${BUILD_DEP}

# deploy depends on the binary so that we're sure it compiles here, at least.
deploy: ${BIN}
	rsync --exclude='${BUILD_DIR}' --delete -av * ${DEPLOY_HOST}:${DEPLOY_DIR}
	ssh ${DEPLOY_HOST} make -C ${DEPLOY_DIR}
	ssh ${DEPLOY_HOST} killall ${BIN} &> /dev/null || exit 0

${BIN}: ${OBJECTS} ${BUILD_DEP}
	${CC} -o $@ ${OBJECTS} ${LDFLAGS}


${BUILD_DIR}/%.o:%.c ${BUILD_DEP}
	${CC} -MMD -o $@ -c $< ${CFLAGS}

.PHONY: clean all

clean:
	rm -rf ${BUILD_DIR}

-include ${DEPS}
