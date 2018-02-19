#define MD_NEWLINE		0x000A
#define MD_CARRIAGE_RETURN	0x000D
#define MD_SPACE		0x0020
#define MD_TAB			0x0009
#define MD_LINE_TABULATION	0x000B
#define MD_FORM_FEED		0x000C

#include "buffer.h"

struct parse_tree;

void parse(struct buffer *buff, size_t len, struct parse_tree *pt);
void render(struct parse_tree *pt, struct buffer *res);
