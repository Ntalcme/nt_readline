#include "buffer.h"
#include <stdlib.h>

#define NT_BUF_INIT_CAP 64

int nt_buffer_init(t_buffer *buf)
{
	char  *data;
	size_t data_cap;

	data_cap = NT_BUF_INIT_CAP;
	data = malloc(data_cap * sizeof(char));
	if (data == NULL)
		return -1;

	buf->data = data;
	buf->cap = data_cap;
	buf->len = 0;

	return 0;
}

int nt_buffer_push(t_buffer *buf, const char c)
{
	char  *new_data;
	size_t new_cap;

	if (buf->len + 1 >= buf->cap) {
		new_cap = buf->cap * 2;
		new_data = realloc(buf->data, new_cap);
		if (new_data == NULL)
			return -1;
		buf->data = new_data;
		buf->cap = new_cap;
	}

	buf->data[buf->len] = c;
	buf->len++;
	return 0;
}

void nt_buffer_pop(t_buffer *buf)
{
	if (buf->len > 0)
		buf->len--;
}

char *nt_buffer_take(t_buffer *buf)
{
	char *str;

	buf->data[buf->len] = '\0';
	str = buf->data;
	buf->data = NULL;
	buf->len = 0;
	buf->cap = 0;

	return str;
}

void nt_buffer_free(t_buffer *buf)
{
	free(buf->data);
	buf->data = NULL;
	buf->len = 0;
	buf->cap = 0;
}
