#include "display.h"
#include <string.h>
#include <unistd.h>

#define NT_ESC "\033"
#define NT_CSI NT_ESC "[" /* Control Sequence Introducer */

#define NT_CURSOR_LEFT  NT_CSI "D"
#define NT_CURSOR_RIGHT NT_CSI "C"
#define NT_ERASE_TO_EOL NT_CSI "K"

static void put(const char *s, size_t len)
{
	write(STDOUT_FILENO, s, len);
}

static void put_str(const char *s)
{
	put(s, strlen(s));
}

void nt_display_cursor_left(size_t n)
{
	size_t i;

	for (i = 0; i < n; i++)
		put_str(NT_CURSOR_LEFT);
}

void nt_display_cursor_right(size_t n)
{
	size_t i;

	for (i = 0; i < n; i++)
		put_str(NT_CURSOR_RIGHT);
}

void nt_display_newline(void)
{
	put("\n", 1);
}

void nt_display_inserted(const t_line_buffer *buf)
{
	size_t tail;

	/* Redraw the inserted char and everything after it */
	tail = buf->len - buf->cursor_pos;
	put(buf->data + buf->cursor_pos - 1, tail + 1);

	/* Move the cursor back, just after the inserted char */
	nt_display_cursor_left(tail);
}

void nt_display_deleted(const t_line_buffer *buf)
{
	size_t tail;

	tail = buf->len - buf->cursor_pos;

	/* Step left, redraw the tail, blank the leftover last char */
	nt_display_cursor_left(1);
	put(buf->data + buf->cursor_pos, tail);
	put(" ", 1);

	/* Put the cursor back at its logical position */
	nt_display_cursor_left(tail + 1);
}

void nt_display_replaced(const t_line_buffer *buf, size_t old_cursor)
{
	/* Go back to the start of the line, erase it, draw the new one */
	nt_display_cursor_left(old_cursor);
	put_str(NT_ERASE_TO_EOL);
	put(buf->data, buf->len);
}
