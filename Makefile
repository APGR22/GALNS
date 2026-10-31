compiler = gcc

include_folder = ./include
main_file = test/main.c

output_file = bin/main

src_files = ${main_file}\
	src/galns.c\
	src/debug.c\
	src/system.c\
	src/nodesystem/nodesystem.c

options = \
	-O2

libs = \
	-lssl\
	-lcrypto

main:
# 	https://stackoverflow.com/a/9967125/22880994
	${compiler} -o ${output_file} ${src_files} -I${include_folder} ${options} ${libs}