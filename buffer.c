#include <stdlib.h>
#include <string.h>

#include "buffer.h"

struct buffer{
	char *data;
	size_t cap, size;
};

struct buffer *buffer_create(){
	struct buffer *buff = (struct buffer*)malloc(sizeof(struct buffer));
	buff->size=0;
	buff->cap=16;
	buff->data = (char*)malloc(buff->cap);
	return buff;
}

size_t buffer_grow(struct buffer *buff){
	size_t orig_cap = buff->cap;
	buff->cap<<=1;
	//printf("Next size: %lu\n", buff->cap);
	buff->data = (char*)realloc(buff->data, buff->cap);
	return orig_cap;
}

size_t buffer_size(struct buffer *buff){return buff->size;}
size_t buffer_resize(struct buffer *buff, size_t size){
	buff->size=size; while(buff->size<buff->cap)buffer_grow(buff);
	return buff->size;
}
size_t buffer_cap(struct buffer *buff){return buff->cap;}
char *buffer_ptr(struct buffer *buff){return buff->data;}

size_t buffer_append(struct buffer *buff, void *obj, size_t len){
	while(buff->size + len >= buff->cap) buffer_grow(buff);
	for(size_t i=0; i<len; i++)buff->data[buff->size+i] = *((char*)obj+i);
	buff->size += len;
	return buff->size;
}
size_t buffer_swap_bytail(struct buffer *buff, int i, size_t len){
	for(size_t j=0; j<len; j++)buff->data[i*len + j] = buff->data[buff->size - len + j];
	buff -> size -= len;
	return buff->size;
}

size_t buffer_pop(struct buffer *buff, size_t len){buff->size-=len; return buff->size;}
char *buffer_top(struct buffer *buff, size_t len){return buff->data + buff->size - len; }

void buffer_free(struct buffer *buff){
	free(buff->data);
	free(buff);
}

