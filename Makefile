compiler = gcc

include_folder := ./include

macro =

libs = \
	-lssl\
	-lcrypto

# https://zenn.dev/atsushifx/articles/devtools-make-mutiplatform
ifeq ($(OS),Windows_NT)
	output_file_ext := .exe
	libs += -lshlwapi
else
	UNAME_OS := $(shell uname -s)
	ifeq ($(UNAME_OS),Linux)
		output_file_ext := .elf
		macro += -D_LARGEFILE64_SOURCE
	else
		output_file_ext := ''
	endif
endif

src_files := \
	src/galns.c\
	src/debug.c\
	src/system.c\
	src/time.c\
	src/nodesystem/nodesystem.c\
	src/nodesystem/nodestream.c

options := \
	-O2

main:
# 	https://stackoverflow.com/a/9967125/22880994
	${compiler} -o bin/main_create${output_file_ext} test/main_create.c ${src_files} -I${include_folder} ${options} ${libs} ${macro}
	${compiler} -o bin/main_load${output_file_ext} test/main_load.c ${src_files} -I${include_folder} ${options} ${libs} ${macro}