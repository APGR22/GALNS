compiler = gcc

include-folder = ./include
main-file = test/main.c

output-file = bin/main

src-files = ${main-file}\
	src/galns.c\
	src/debug.c

options = \
	-O2

main:
# 	https://stackoverflow.com/a/9967125/22880994
	${compiler} -o ${output-file} ${src-files} -I${include-folder} ${options}