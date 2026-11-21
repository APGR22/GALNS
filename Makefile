CC = gcc
CFLAGS := \
	-O2

SRCS := \
	src/galns.c\
	src/debug.c\
	src/system.c\
	src/nodesystem/nodesystem.c\
	src/nodesystem/nodestream.c

INCLUDE := ./include

MACROS =

LIBS = \
	-lssl\
	-lcrypto

EXEC_FILE_EXT =
LIBS_FILE_EXT =

MKDIR =

# https://zenn.dev/atsushifx/articles/devtools-make-mutiplatform
ifeq ($(OS),Windows_NT)
	MKDIR := mkdir

	EXEC_FILE_EXT = .exe
	LIBS_FILE_EXT = .dll

	LIBS += -lshlwapi
else
	UNAME_OS := $(shell uname -s)
	ifeq ($(UNAME_OS),Linux)
		MKDIR = mkdir -p

		EXEC_FILE_EXT = .elf
		LIBS_FILE_EXT = .so
	
		MACROS += -D_LARGEFILE64_SOURCE
	else
		MKDIR = ''
	endif
endif

all: lib

lib:
#	https://stackoverflow.com/a/20566812/22880994
#	https://stackoverflow.com/a/47828799/22880994
ifeq ("$(wildcard bin)","")
	${MKDIR} bin
endif
	${CC} -o bin/galns${LIBS_FILE_EXT} ${SRCS} -I${INCLUDE} ${CFLAGS} -shared -fPIC ${LIBS} ${MACROS} -DGALNS_API_EXPORT=1

test: test/main_create.c test/main_load.c
ifeq ("$(wildcard bin)","")
	${MKDIR} bin
endif
ifeq ("$(wildcard .dumptest)","")
	${MKDIR} .dumptest
endif
# 	https://stackoverflow.com/a/9967125/22880994
	${CC} -o bin/main_create${EXEC_FILE_EXT} test/main_create.c ${SRCS} -I${INCLUDE} ${CFLAGS} ${LIBS} ${MACROS}
	${CC} -o bin/main_load${EXEC_FILE_EXT} test/main_load.c ${SRCS} -I${INCLUDE} ${CFLAGS} ${LIBS} ${MACROS}
	bin/main_create${EXEC_FILE_EXT}
	bin/main_load${EXEC_FILE_EXT}