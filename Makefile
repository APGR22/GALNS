compiler = gcc

include_folder := ./include
main_file := test/main.c

# https://zenn.dev/atsushifx/articles/devtools-make-mutiplatform
ifeq ($(OS),Windows_NT)
	output_file := bin/main.exe
else
	UNAME_OS := $(shell uname -s)
	ifeq ($(UNAME_OS),Linux)
		output_file := bin/main.elf
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

libs := \
	-lssl\
	-lcrypto

main:
# 	https://stackoverflow.com/a/9967125/22880994
	${compiler} -o ${output_file} ${src_files} -I${include_folder} ${options} ${libs}