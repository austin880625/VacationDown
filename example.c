#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "markdown.h"

char doc[131072];
char filename[] = "doc2.md";

int main(){
	FILE *fp = fopen(filename, "r");
	size_t len=0;
	char c;
	while((c = getc(fp)) != EOF){
		putchar(c);
		doc[len++] = c;
	}

	struct parse_tree *res = parse_tree_create();
	puts("Start parsing...");
	parse(doc, len, res);
	puts("Below is parse tree");
	print_parse_tree(res);

	fclose(fp);
	return 0;
}
