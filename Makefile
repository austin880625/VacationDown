all: source
	gcc -o a.out buffer.o utf8.o markdown.o example.o
source: buffer.c markdown.c example.c utf8.c
	gcc -c -g -Wall buffer.c
	gcc -c -g -Wall utf8.c
	gcc -c -g -Wall markdown.c
	gcc -c -g -Wall example.c
