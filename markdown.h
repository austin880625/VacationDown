#ifndef __MD_FILE
#define __MD_FILE

#define MD_NEWLINE		0x000A
#define MD_CARRIAGE_RETURN	0x000D
#define MD_SPACE		0x0020
#define MD_TAB			0x0009
#define MD_LINE_TABULATION	0x000B
#define MD_FORM_FEED		0x000C
#define MD_GT			0x003E
#define MD_ASTERISK		0x002A
#define MD_PLUS			0x002B
#define MD_MINUS		0x002D
#define MD_DOT			0x002E
#define MD_PAREN		0x0029
#define MD_UNDERLINE		0x005F
#define MD_NUMBER		0x0023
#define MD_BACKTICK		0x0060
#define MD_TILDE		0x007E

#define MD_DIG_ZERO		0x0030
#define MD_DIG_NINE		0x0039

#define MD_LIST_TYPE_ASTERISK	0x0001
#define MD_LIST_TYPE_PLUS	0x0002
#define MD_LIST_TYPE_MINUS	0x0003
#define MD_LIST_TYPE_DOT	0x0004
#define MD_LIST_TYPE_PAREN	0x0005
#define MD_LIST_TYPE_ONE	0x1000

#define MD_ESCAPE		0x005C


#include <stdlib.h>
#include "buffer.h"

enum node_type {
	DOC,
	CONT_BLOCKQUOTE,
	CONT_LIST_ITEM,
	// CONT_LIST_OL and CONT_LIST_UL is used when rendering
	CONT_LIST_OL,
	CONT_LIST_UL,
	LEAF_THEMATIC_BREAK,
	ATX_H1, ATX_H2, ATX_H3, ATX_H4, ATX_H5, ATX_H6,
	LEAF_SET_TEXT_HEADING,
	LEAF_INDENTED_CODE_BLOCK,
	LEAF_FENCED_CODE_BLOCK,
	LEAF_HTML_BLOCK,
	LEAF_LINE_REFERECE,
	LEAF_PARAGRAPH,
	LEAF_BLANK_LINE,
	INLI_ESCAPE,
	INLI_ENTITY,
	INLI_CODE_SPAN,
	INLI_EMPHASIZE,
	INLI_STRONG_EMPHASIZE,
	INLI_LINK,
	INLI_IMAGE,
	INLI_AUTOLINK,
	INLI_RAW_HTML,
	INLI_HARD_LINE_BREAK,
	INLI_SOFT_LINE_BREAK,
	INLI_TEXTUAL,
	CONT_LIST,
	LEAF_ATX_HEADING,
	UNDET
};

struct parse_tree;
void print_parse_tree(struct parse_tree *o);
struct parse_tree *parse_tree_create();
void parse(char *doc, size_t len, struct parse_tree *res);
size_t render(struct parse_tree *pt, char **res);

#endif
