LIBS=libpng tslib

NAME=desco

DEPLOY_HOST?=desco
DEPLOY_DIR?=/root/desco

SUBDIRS = graphics graphics/font input text

include base.mk

# deploy depends on the binary so that we're sure it compiles here, at least.
deploy: ${BIN}
	rsync --exclude='${BUILD_DIR}' --delete -av * ${DEPLOY_HOST}:${DEPLOY_DIR}
	ssh ${DEPLOY_HOST} make -C ${DEPLOY_DIR}
	ssh ${DEPLOY_HOST} killall ${BIN} &> /dev/null || exit 0
