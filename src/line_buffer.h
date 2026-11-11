#ifndef NT_LINE_BUFFER_H
#define NT_LINE_BUFFER_H

#include <stddef.h>

typedef struct s_line_buffer {
	char  *data;
	size_t len;
	size_t cap;
	size_t cursor_pos;
} t_line_buffer;

int   nt_line_buffer_init(t_line_buffer *buf);
char *nt_line_buffer_take(t_line_buffer *buf);
void  nt_line_buffer_free(t_line_buffer *buf);
int   nt_line_buffer_insert(t_line_buffer *buf, const char c);
void  nt_line_buffer_delete(t_line_buffer *buf);
int   nt_line_buffer_set(t_line_buffer *buf, const char *str);

#endif
