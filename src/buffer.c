#include "buffer.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define NT_BUF_INIT_CAP 64

int nt_buffer_init(t_buffer *buf)
{
	char  *data;
	size_t data_cap;

	data_cap = NT_BUF_INIT_CAP;
	data     = malloc(data_cap * sizeof(char));
	if (data == NULL)
		return -1;

	buf->data       = data;
	buf->cap        = data_cap;
	buf->len        = 0;
	buf->cursor_pos = 0;

	return 0;
}

char *nt_buffer_take(t_buffer *buf)
{
	char *str;

	buf->data[buf->len] = '\0';
	str                 = buf->data;
	buf->data           = NULL;
	buf->len            = 0;
	buf->cap            = 0;
	buf->cursor_pos     = 0;

	return str;
}

void nt_buffer_free(t_buffer *buf)
{
	free(buf->data);
	buf->data       = NULL;
	buf->len        = 0;
	buf->cap        = 0;
	buf->cursor_pos = 0;
}

int nt_buffer_insert(t_buffer *buf, const char c)
{
	char  *new_data;
	size_t new_cap;

	if (buf->len + 1 >= buf->cap) {
		new_cap  = buf->cap * 2;
		new_data = realloc(buf->data, new_cap);
		if (new_data == NULL)
			return -1;
		buf->data = new_data;
		buf->cap  = new_cap;
	}

	/* Shift the tail right by one (overlapping areas, see memmove(3)) */
	memmove(buf->data + buf->cursor_pos + 1, buf->data + buf->cursor_pos,
	        buf->len - buf->cursor_pos);

	buf->data[buf->cursor_pos] = c;
	buf->len++;
	buf->cursor_pos++;
	return 0;
}

void nt_buffer_delete(t_buffer *buf)
{
	if (buf->cursor_pos == 0)
		return;

	/* Shift the tail left by one (overlapping areas, see memmove(3)) */
	memmove(buf->data + buf->cursor_pos - 1, buf->data + buf->cursor_pos,
	        buf->len - buf->cursor_pos);
	buf->len--;
	buf->cursor_pos--;
}
