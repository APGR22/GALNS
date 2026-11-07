compiler = gcc

include_folder := ./include
main_file := test/main.c

macro =

libs = \
	-lssl\
	-lcrypto

# https://zenn.dev/atsushifx/articles/devtools-make-mutiplatform
ifeq ($(OS),Windows_NT)
	output_file := bin/main.exe
	libs += -lshlwapi
else
	UNAME_OS := $(shell uname -s)
	ifeq ($(UNAME_OS),Linux)
		output_file := bin/main.elf
		macro += -D_LARGEFILE64_SOURCE
	else
		output_file := bin/main
	endif
endif

src_files := ${main_file}\
	src/galns.c\
	src/debug.c\
	src/system.c\
	src/nodesystem/nodesystem.c\
	src/nodesystem/nodestream.c

options := \
	-O2

main:
# 	https://stackoverflow.com/a/9967125/22880994
	${compiler} -o ${output_file} ${src_files} -I${include_folder} ${options} ${libs} ${macro}