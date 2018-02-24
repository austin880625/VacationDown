#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "utf8.h"
#include "markdown.h"

char *doc;
char *conv;
char filename[] = "README.md";

int main(){
	FILE *fp = fopen(filename, "r");
	size_t len=0;
	fseek(fp, 0, SEEK_END);
	len = (size_t)ftell(fp);
	rewind(fp);
	doc = (char*)malloc(len+1);
	fread(doc, 1, len, fp);
	conv = (char*)malloc(len*2);
	size_t new_len = convfrom_utf8((unsigned char*) conv, (unsigned char*)doc, len);

	struct parse_tree *res = parse_tree_create();
	//puts("Start parsing...");
	parse(conv, new_len, res);
	//puts("Below is parse tree");
	//print_parse_tree(res);
	//puts("Rendering...");
	char *res_html;
	len = render(res, &res_html);
	for(size_t i=0; i<len; i++)putchar(res_html[i]);

	fclose(fp);
	return 0;
}
