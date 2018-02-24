#include <stdlib.h>
struct buffer;
// buffer_create: create a buffer object and return its pointer
struct buffer *buffer_create();
// buffer_grow: double the buffer's capacity
size_t buffer_grow(struct buffer *buff);
// buffer_append: copy len bytes of value from pointer obj
size_t buffer_append(struct buffer *buff, void *obj, size_t len);
// buffer_swap_bytail: delete the ith element and swap by the last element
size_t buffer_swap_bytail(struct buffer *buff, int i, size_t len);
size_t buffer_cap(struct buffer *buff);
size_t buffer_size(struct buffer *buff);
size_t buffer_resize(struct buffer *buff, size_t size);
char *buffer_ptr(struct buffer *buff);

size_t buffer_pop(struct buffer *buff, size_t len);
char *buffer_top(struct buffer *buff, size_t len);
// buffer_free: free the buffer object and its data
void buffer_free(struct buffer *buff);
