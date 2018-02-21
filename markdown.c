#include "markdown.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define DEBUG
#ifdef DEBUG
	#include <stdio.h>
#endif

struct parse_tree{
	enum node_type t;
	// attr: if nonzero on list_item means the number of whitespace
	// indentation for matching
	// if on list means the list type
	uint16_t attr;
	struct buffer *text;
	struct parse_tree *next;
	struct parse_tree *children;
};
void parse_tree_init(struct parse_tree *o){
	o->t = UNDET;
	o->attr = 0;
	o->text = NULL;
	o->next = NULL;
	o->children = NULL;
}

#ifdef DEBUG
static char *enum_strings[] = {"DOC","CONT_BLOCKQUOTE","CONT_LIST_ITEM","CONT_LIST","LEAF_THEMATIC_BREAK","LEAF_ATX_HEADING",
	"LEAF_SET_TEXT_HEADING","LEAF_INDENTED_CODE_BLOCK","LEAF_FENCED_CODE_BLOCK","LEAF_HTML_BLOCK","LEAF_LINE_REFERECE",
	"LEAF_PARAGRAPH","LEAF_BLANK_LINE","INLI_ESCAPE","INLI_ENTITY","INLI_CODE_SPAN","INLI_EMPHASIZE","INLI_STRONG_EMPHASIZE",
	"INLI_LINK","INLI_IMAGE","INLI_AUTOLINK","INLI_RAW_HTML","INLI_HARD_LINE_BREAK","INLI_SOFT_LINE_BREAK","INLI_TEXTUAL",
	"UNDET" };
void print_rec(struct parse_tree *o, int dep)
{
	for(int i=0; i<dep-1; i++){printf("| ");}
	if(dep)printf("-");
	printf("%s %04x\n", enum_strings[o->t], o->attr);	
	for(struct parse_tree *cur = o->children; cur; cur=cur->next){
		print_rec(cur, dep+1);
	}
}
void print_parse_tree(struct parse_tree *o){ print_rec(o, 0); printf("\n\n"); }
#else
void print_parse_tree(struct parse_tree *o){ o=NULL; }
#endif
#ifdef REV
static inline uint16_t get_ucs(char *doc, size_t pos){ return ((uint16_t)doc[pos]<<8)|((uint16_t)doc[pos+1]); }
#else
static inline uint16_t get_ucs(char *doc, size_t pos){ return ((uint16_t)doc[pos+1]<<8)|((uint16_t)doc[pos]); }
#endif
// is_ prefix function are used for new open blocks
// by matching prefixes
//
int is_blankline(char *doc, size_t line_beg)
{
	int res = 0;
	uint16_t c = get_ucs(doc, line_beg);
	while(c == MD_SPACE || c == MD_TAB){ res+=2; line_beg+=2; c=get_ucs(doc, line_beg); }
	if(c == MD_NEWLINE)return res+2;
	if(c == MD_CARRIAGE_RETURN){
		if(get_ucs(doc, line_beg+2) == MD_NEWLINE)return res+4;
		else return res+2;
	}
	return -1;
}
int is_blockquote(char *doc, size_t line_beg)
{
	int res = 0;
	uint16_t c = get_ucs(doc, line_beg+res);
	if(c == MD_SPACE){ res+=2; c = get_ucs(doc, line_beg+res); }
	if(c == MD_SPACE){ res+=2; c = get_ucs(doc, line_beg+res); }
	if(c == MD_SPACE){ res+=2; c = get_ucs(doc, line_beg+res); }
	if(c == MD_GT){
		res+=2;
		c = get_ucs(doc, line_beg+res);
		if(c == MD_SPACE) res+=2;
		return res;
	}
	return -1;
}
int is_thematic_break(char *doc, size_t line_beg)
{
	int i=0;
	if(get_ucs(doc, line_beg+i) == MD_SPACE)i+=2;
	if(get_ucs(doc, line_beg+i) == MD_SPACE)i+=2;
	if(get_ucs(doc, line_beg+i) == MD_SPACE)i+=2;
	uint16_t c = get_ucs(doc, line_beg+i); i+=2;
	int cnt=1;
	uint16_t cur = get_ucs(doc, line_beg+i);
	while(cur != MD_NEWLINE && cur != MD_CARRIAGE_RETURN){
		if(cur != MD_SPACE){
			if(cur != c)return -1;
			cnt++;
		}
		i+=2; cur = get_ucs(doc, line_beg+i);
	}
	if(cnt<3)return -1;
	if(!(c == MD_ASTERISK || c == MD_MINUS || c == MD_UNDERLINE))return -1;
	return i;
}
int is_list_item(char *doc, size_t line_beg, uint16_t *list_type, struct parse_tree *cur_open)
{
	// matching indentation of the list_item block
	if(list_type == NULL){
		int i;
		if(is_blankline(doc, line_beg) != -1)return 0;
		for(i=0; i<cur_open->attr; i+=2){
			if(get_ucs(doc, line_beg+i) != MD_SPACE)return -1;
		}
		return i;
	}
	// matching a list marker
	else{
		int orig_beg = line_beg;
		// unordered list marker
		if(get_ucs(doc, line_beg) == MD_SPACE)line_beg+=2;
		if(get_ucs(doc, line_beg) == MD_SPACE)line_beg+=2;
		if(get_ucs(doc, line_beg) == MD_SPACE)line_beg+=2;
		uint16_t c = get_ucs(doc, line_beg);
		int w=0;
		if(c == MD_ASTERISK || c == MD_PLUS || c == MD_MINUS){
			if(c == MD_ASTERISK) *list_type = MD_LIST_TYPE_ASTERISK;
			else if(c == MD_PLUS) *list_type = MD_LIST_TYPE_PLUS;
			else if(c == MD_MINUS) *list_type = MD_LIST_TYPE_MINUS;
		}
		else if(MD_DIG_ZERO <= c && c <= MD_DIG_NINE){
			while( MD_DIG_ZERO <= c && c <= MD_DIG_NINE){w+=2; c=get_ucs(doc, line_beg+w);}
			if(c != MD_PAREN && c != MD_DOT) return -1;
			if(c == MD_PAREN) *list_type = MD_LIST_TYPE_PAREN;
			if(c == MD_DOT) *list_type = MD_LIST_TYPE_DOT;
			if(w == 2 && get_ucs(doc, line_beg) == MD_DIG_ZERO+1) *list_type&=MD_LIST_TYPE_ONE;
		}else
			return -1;
		w+=2;
		int n;
		for(n=0; n<=10; n+=2){if(get_ucs(doc, line_beg+w+n) != MD_SPACE)break;}
		if(n == 0){
			// An empty list
			if(is_blankline(doc, line_beg+w+n) != -1)
				return line_beg-orig_beg+w+n;
			return -1;
		}
		if(n == 12)return line_beg-orig_beg+w+2;
		return line_beg-orig_beg+w+n;
	}
}
int is_atx_heading(char *doc, size_t line_beg, struct parse_tree *cur_child)
{
	int i=0;
	if(get_ucs(doc, line_beg+i) == MD_SPACE)i+=2;
	if(get_ucs(doc, line_beg+i) == MD_SPACE)i+=2;
	if(get_ucs(doc, line_beg+i) == MD_SPACE)i+=2;
	uint16_t c = get_ucs(doc, line_beg);
	int level = 0;
	while(c == MD_NUMBER){ level++; i+=2; c = get_ucs(doc, line_beg+i);}
	if(c != MD_SPACE || level>6){ return -1; }
	if(cur_child)cur_child->attr = level;
	return i+2;
}

int match_marker(char *doc, size_t line_beg, struct buffer *stack, struct parse_tree **matched)
{
	size_t szptp = sizeof(struct parse_tree *);
	int res = 0;
	for(size_t i=0; i<buffer_size(stack); i+=szptp){
		struct parse_tree *anc = *(struct parse_tree**)(buffer_ptr(stack)+i);
		if(anc->t == CONT_BLOCKQUOTE){
			int prefix = is_blockquote(doc, line_beg+res);
			if(prefix != -1)res+=prefix;
			else { *matched = *(struct parse_tree**)(buffer_ptr(stack)+i-szptp); return res; }
		}
		if(anc->t == CONT_LIST_ITEM){
			int prefix = is_list_item(doc, line_beg+res, NULL, anc);
			if(prefix != -1)res+=prefix;
			else { *matched = *(struct parse_tree**)(buffer_ptr(stack)+i-2*szptp); return res; }
		}
		if(anc->t == LEAF_PARAGRAPH){
			// Never match paragraph since we need to check for a new opening
			assert(anc == *(struct parse_tree **)(buffer_top(stack,szptp)));
			*matched = *(struct parse_tree**)(buffer_ptr(stack)+i-szptp);
			return res;
		}
	}
	*matched = *(struct parse_tree **)buffer_top(stack, szptp);
	return res;
}

// keep_open: determine whether the line keep the cur_open block open
// If so, return the offset of the line to parse the containing block
// otherwise return -1
int keep_open(char *doc, size_t line_beg, struct parse_tree *cur_open, struct buffer *stack, struct parse_tree **matched)
{
	// The DOC node is always open til reaching the end
	// of the document
	if(cur_open->t == DOC){ *matched = cur_open; return 0; }
	// Some node can be closed immediately since they're consist of only one line
	if(cur_open->t == LEAF_BLANK_LINE ||
	   cur_open->t == LEAF_THEMATIC_BREAK ||
	   cur_open->t == LEAF_ATX_HEADING
	   ) return -1;
	int res = match_marker(doc, line_beg, stack, matched);
	if(cur_open->t == CONT_LIST && (is_list_item(doc, line_beg, NULL, cur_open)==-1) )return -1;
	if(*matched == cur_open){ return res; }
	else if(cur_open->t != LEAF_PARAGRAPH){ return -1; }
	// There're still unmatched open blocks, but need to
	// check if there's any block opening to determine
	// whether the block should be closed.
	else return res;
}

int new_open(char *doc, size_t line_beg, struct parse_tree *cur_open, struct parse_tree *cur_child)
{
	int res = 0;
	res = is_blockquote(doc, line_beg);
	if(res != -1){ if(cur_child) cur_child->t = CONT_BLOCKQUOTE; return res;}
	res = is_thematic_break(doc, line_beg);
	if(res != -1){ if(cur_child) cur_child->t = LEAF_THEMATIC_BREAK;  return res; }
	res = is_atx_heading(doc, line_beg, cur_child);
	if(res != -1){ if(cur_child) cur_child->t = LEAF_ATX_HEADING; return res; }
	uint16_t list_type;
	res = is_list_item(doc, line_beg, &list_type, cur_open);
	if(res != -1){
		// Blank list cannot interrupt a paragraph
		if(cur_open->t == LEAF_PARAGRAPH && (is_blankline(doc, line_beg+res) != -1)){return -1;}
		if(cur_open->t == LEAF_PARAGRAPH && !(list_type&MD_LIST_TYPE_ONE)){
			if(list_type != MD_LIST_TYPE_PLUS && list_type !=MD_LIST_TYPE_MINUS && list_type != MD_LIST_TYPE_ASTERISK)
				return -1;
		}
		// Check if it a new list
		if(cur_open->t == CONT_LIST && cur_open->attr == list_type){
			if(cur_child){
				cur_child->t = CONT_LIST_ITEM;
				cur_child->attr = (uint16_t)res;
			}
			return res;
		}
		else{
			if(cur_child){
				cur_child->t = CONT_LIST;
				cur_child->attr = list_type;
			}
			return 0;
		}
	}
	if(cur_open->t == LEAF_FENCED_CODE_BLOCK ||
	   cur_open->t == LEAF_INDENTED_CODE_BLOCK ||
	   cur_open->t == LEAF_HTML_BLOCK ||
	   cur_open->t == LEAF_ATX_HEADING ||
	   cur_open->t == LEAF_THEMATIC_BREAK
	  )return -1;
	res = is_blankline(doc, line_beg);
	if(res != -1) {
		if(cur_open->t == LEAF_BLANK_LINE)return -1;
		if(cur_child) cur_child->t = LEAF_BLANK_LINE; 
		return 0; 
	}

	// A paragraph, determine whether we should create a paragraph
	if(cur_open->t == LEAF_PARAGRAPH)return -1;

	if(cur_child) cur_child->t = LEAF_PARAGRAPH;
	return 0;
}

// move to next line
size_t add_line(char *doc, size_t line_beg, size_t len, struct parse_tree *cur_open)
{
	size_t end = line_beg;
	while(end<len && get_ucs(doc, end) != MD_NEWLINE && get_ucs(doc, end) != MD_CARRIAGE_RETURN){end+=2;}
	if(get_ucs(doc, end) == MD_CARRIAGE_RETURN && get_ucs(doc, end+2) == MD_NEWLINE)end+=2;
	if(!cur_open->text)cur_open->text = buffer_create();
	
	buffer_append(cur_open->text, doc+line_beg, (size_t)((int)end-(int)line_beg+2));
	return (size_t)((int)end-(int)line_beg+2);
}

struct parse_tree *parse_tree_create(){
	struct parse_tree *res = (struct parse_tree *)malloc(sizeof(struct parse_tree)); parse_tree_init(res);
	return res;
}
void parse(char *doc, size_t len, struct parse_tree *res) 
{
	size_t szpt = sizeof(struct parse_tree);
	size_t szptp = sizeof(struct parse_tree *);
	res->t = DOC;
	res->children = (struct parse_tree *)malloc(szpt);
	parse_tree_init(res->children);
	struct buffer *stack = buffer_create();
	buffer_append(stack, &res, szptp);
	struct parse_tree *cur_child = res->children;

	// Phase 1: parse for each line into parse tree of blocks
	size_t line_beg = 0;
	while(line_beg < len){
		// Determine whether the line closes the current open blocks
		struct parse_tree *cur_open = *(struct parse_tree **)buffer_top(stack, szptp);
		struct parse_tree *matched;
		int ko = keep_open(doc, line_beg, cur_open, stack, &matched);
		while(ko == -1){
			cur_child = cur_open;
			cur_child->next = (struct parse_tree*) malloc(szpt);
			cur_child = cur_child->next; parse_tree_init(cur_child);
			buffer_pop(stack, szptp);
			cur_open = *(struct parse_tree **)buffer_top(stack, szptp);
			ko = keep_open(doc, line_beg, cur_open, stack, &matched);
		}
		line_beg += (size_t)ko;
		// Determine whether the line opens a new block.
		// If it doesn't and current open block is a leaf block, it always
		// returns -1 and leave cur_child empty
		ko = new_open(doc, line_beg, cur_open, NULL);
		// Close all unmatched blocks before opening a new block
		if(ko != -1){
			while(cur_open != matched){
				cur_child = cur_open;
				cur_child->next = (struct parse_tree*) malloc(szpt);
				cur_child = cur_child->next; parse_tree_init(cur_child);
				buffer_pop(stack, szptp);
				cur_open = *(struct parse_tree **)buffer_top(stack, szptp);
			}
		}
		// Do it again to obtain info of cur_child since it might been
		// overwritten when popping the stack
		ko = new_open(doc, line_beg, cur_open, cur_child);
		while(ko != -1){
			buffer_append(stack, &cur_child, szptp);
			cur_open = cur_child;
			cur_open->children = (struct parse_tree *)malloc(szpt);
			cur_child = cur_open->children; parse_tree_init(cur_child);
			line_beg += (size_t)ko;
			ko = new_open(doc, line_beg, cur_open, cur_child);
		}
		// At here, we reached the deepest open block and
		// cur_child is empty, the remain of the line is the
		// text to be added to the cur_open block(which is a leaf)
		line_beg += add_line(doc, line_beg, len, cur_open);
		printf("%lu\n", line_beg);
		print_parse_tree(res);
	}

	// Phase 2: parse the inline elements, we break the text field 
	// into children of the leaf block with type INLI_XX in field t
}
