#include "keys.h"
#include "display.h"
#include "history.h"
#include <unistd.h>

#define NT_KEY_ESC    27
#define NT_KEY_DEL    127
#define NT_KEY_CTRL_D 4
#define NT_KEY_CTRL_H 8

#define NT_ARROW_UP    'A'
#define NT_ARROW_DOWN  'B'
#define NT_ARROW_RIGHT 'C'
#define NT_ARROW_LEFT  'D'

/** @brief Insert a printable char and redraw. */
static t_result handle_printable(t_line_buffer *buf, unsigned char c)
{
	if (nt_line_buffer_insert(buf, c) == -1)
		return NT_ERROR;

	nt_display_inserted(buf);
	return NT_CONTINUE;
}

/** @brief Delete the char before the cursor and redraw. */
static t_result handle_backspace(t_line_buffer *buf)
{
	if (buf->cursor_pos == 0)
		return NT_CONTINUE;

	nt_line_buffer_delete(buf);
	nt_display_deleted(buf);
	return NT_CONTINUE;
}

/** @brief Return NT_EOF on an empty line, else ignore. */
static t_result handle_ctrl_d(const t_line_buffer *buf)
{
	/* Ctrl+D on a non-empty line is ignored */
	if (buf->len > 0)
		return NT_CONTINUE;

	nt_display_newline();
	return NT_EOF;
}

/** @brief Handle an arrow-key escape sequence. */
static t_result handle_escape(t_line_buffer *buf, t_nav *nav)
{
	unsigned char seq[2];
	t_result      res;

	if (read(STDIN_FILENO, &seq[0], 1) != 1 ||
	    read(STDIN_FILENO, &seq[1], 1) != 1)
		return NT_ERROR;

	if (seq[0] != '[')
		return NT_CONTINUE;

	res = NT_CONTINUE;
	switch (seq[1]) {
	case NT_ARROW_UP:
		res = nt_history_move(buf, nav, NT_HIST_PREV);
		break;

	case NT_ARROW_DOWN:
		res = nt_history_move(buf, nav, NT_HIST_NEXT);
		break;

	case NT_ARROW_LEFT:
		if (buf->cursor_pos > 0) {
			buf->cursor_pos--;
			nt_display_cursor_left(1);
		}
		break;

	case NT_ARROW_RIGHT:
		if (buf->cursor_pos < buf->len) {
			buf->cursor_pos++;
			nt_display_cursor_right(1);
		}
		break;
	}

	return res;
}

/** @brief Take the line out of buf and add it to the history. */
static t_result handle_enter(t_line_buffer *buf, char **line)
{
	nt_display_newline();

	*line = nt_line_buffer_take(buf);
	if (*line == NULL)
		return NT_ERROR;

	nt_history_add(*line);
	return NT_DONE;
}

t_result nt_key_dispatch(unsigned char c, t_line_buffer *buf, t_nav *nav,
                         char **line)
{
	/* Printable characters, see ascii(7) */
	if (c >= ' ' && c <= '~')
		return handle_printable(buf, c);
	if (c == NT_KEY_CTRL_H || c == NT_KEY_DEL)
		return handle_backspace(buf);
	if (c == NT_KEY_CTRL_D)
		return handle_ctrl_d(buf);
	if (c == NT_KEY_ESC)
		return handle_escape(buf, nav);
	if (c == '\n')
		return handle_enter(buf, line);

	/* Any other key is ignored */
	return NT_CONTINUE;
}
