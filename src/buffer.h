#ifndef NT_BUFFER_H
#define NT_BUFFER_H

#include <stddef.h>

typedef struct s_buffer {
	char  *data;
	size_t len;
	size_t cap;
} t_buffer;

int nt_buffer_init(t_buffer *buf);
int nt_buffer_push(t_buffer *buf, const char c);
void nt_buffer_pop(t_buffer *buf);
char *nt_buffer_take(t_buffer *buf);
void nt_buffer_free(t_buffer *buf);

#endif
