#include "markdown.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

struct parse_tree{
	enum node_type t;
	char *text;
	struct parse_tree *next;
	struct parse_tree *children;
};

inline uint16_t get_ucs(char *doc, size_t pos){ return ((uint16_t)doc[pos]<<8)|(uint16_t)doc[pos+1]; }

size_t is_blankline(char *doc, size_t line_beg)
{
	uint16_t c = get_ucs(doc, line_beg);
	while(c == MD_SPACE || c == MD_TAB){ line_beg+=2; c=get_ucs(doc, line_beg); }
	if(c == MD_NEWLINE)return line_beg+2;
	if(c == MD_CARRIAGE_RETURN){
		if(get_ucs(doc, line_beg+2) == MD_NEWLINE)return line_beg+4;
		else return line_beg+2;
	}
	return 0;
}

// keep_open: determine whether the line keep the cur_open block open
// If so, return the offset of the line to parse the containing block
// otherwise return -1
size_t keep_open(char *doc, size_t line_beg, const struct parse_tree *cur_open)
{
	size_t szpt = sizeof(struct parse_tree);
	// the DOC node is always open til reaching the end
	// of the document
	if(cur_open->t == DOC)return 0;

	// the BLOCKQUOTE container
	if(cur_open->t == CONT_BLOCKQUOTE){
		if(is_blankline(doc, line_beg))return -1;
	}
}

size_t new_open(char *doc, size_t len, struct parse_tree *cur_open, struct parse_tree *cur_child);

size_t add_line(char *doc, size_t len, struct parse_tree *cur_open);

void parse(char *doc, size_t len, struct parse_tree *res) 
{
	size_t szpt = sizeof(struct parse_tree);
	res = (struct parse_tree *)malloc(szpt);
	res->t = DOC;
	res->children = (struct parse_tree *)malloc(szpt);
	struct buffer *stack = buffer_create();
	buffer_append(stack, res, szpt);
	struct parse_tree *cur_child = res->children;

	// Phase 1: parse for each line into parse tree of blocks
	size_t line_beg = 0;
	while(line_beg < len){
		// determine whether the line closes the current open blocks
		struct parse_tree *cur_open = (struct parse_tree *)buffer_top(stack, szpt);
		size_t ko = keep_open(doc, line_beg, cur_open);
		while(ko == -1){
			cur_child = cur_open;
			buffer_pop(stack, szpt);
			cur_child->next = (struct parse_tree*) malloc(szpt);
			cur_child = cur_child->next;
			cur_open = (struct parse_tree *)buffer_top(stack, szpt);
			ko = keep_open(doc, line_beg, cur_open);
		}
		line_beg += ko;
		// determine whether the line opens a new block
		// if current open block is a leaf block, it always
		// returns -1 and leave cur_child empty
		ko = new_open(doc, line_beg, cur_open, cur_child);
		while(ko != -1){
			buffer_append(stack, cur_child, szpt);
			cur_open = cur_child;
			cur_open->children = (struct parse_tree *)malloc(szpt);
			cur_child = cur_open->children;
			line_beg += ko;
			ko = new_open(doc, line_beg, cur_open, cur_child);
		}
		// At here, we reached the deepest open block and
		// cur_child is empty, the remain of the line is the
		// text to be added to the cur_open block
		line_beg += add_line(doc, line_beg, cur_open);
	}

	// Phase 2: parse the inline elements, we break the text field 
	// into children of the leaf block with INLI_ prefix in field t
}
