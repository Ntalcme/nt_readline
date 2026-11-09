#include "nt_readline.h"
#include "buffer.h"
#include "terminal.h"
#include <stdio.h>
#include <unistd.h>

#define NT_KEY_ESC    27
#define NT_KEY_DEL    127
#define NT_KEY_CTRL_D 4
#define NT_KEY_CTRL_H 8

typedef enum e_result {
	NT_CONTINUE, /* keep reading */
	NT_DONE,     /* Enter pressed, line is ready */
	NT_EOF,      /* Ctrl+D on an empty line, or input closed */
	NT_ERROR,    /* read or allocation failure */
} t_result;

static t_result handle_printable(t_buffer *buf, unsigned char c)
{
	size_t tail;
	size_t i;

	if (nt_buffer_insert(buf, c) == -1)
		return NT_ERROR;

	/* Redraw the inserted char and everything after it */
	tail = buf->len - buf->cursor_pos;
	write(STDOUT_FILENO, buf->data + buf->cursor_pos - 1, tail + 1);

	/* Move the cursor back, just after the inserted char */
	for (i = 0; i < tail; i++)
		write(STDOUT_FILENO, "\033[D", 3);

	return NT_CONTINUE;
}

static t_result handle_backspace(t_buffer *buf)
{
	size_t tail;
	size_t i;

	if (buf->cursor_pos == 0)
		return NT_CONTINUE;

	nt_buffer_delete(buf);
	tail = buf->len - buf->cursor_pos;

	/* Step left, redraw the tail, blank the leftover last char */
	write(STDOUT_FILENO, "\b", 1);
	write(STDOUT_FILENO, buf->data + buf->cursor_pos, tail);
	write(STDOUT_FILENO, " ", 1);

	/* Put the cursor back at its logical position */
	for (i = 0; i < tail + 1; i++)
		write(STDOUT_FILENO, "\033[D", 3);

	return NT_CONTINUE;
}

static t_result handle_ctrl_d(const t_buffer *buf)
{
	/* Ctrl+D on a non-empty line is ignored */
	if (buf->len > 0)
		return NT_CONTINUE;

	write(STDOUT_FILENO, "\n", 1);
	return NT_EOF;
}

static t_result handle_escape(t_buffer *buf)
{
	unsigned char seq[2];

	if (read(STDIN_FILENO, &seq[0], 1) != 1 ||
	    read(STDIN_FILENO, &seq[1], 1) != 1)
		return NT_ERROR;

	if (seq[0] != '[')
		return NT_CONTINUE;

	switch (seq[1]) {
	case 'D': /* left */
		if (buf->cursor_pos > 0) {
			buf->cursor_pos--;
			write(STDOUT_FILENO, "\033[D", 3);
		}
		break;

	case 'C': /* right */
		if (buf->cursor_pos < buf->len) {
			buf->cursor_pos++;
			write(STDOUT_FILENO, "\033[C", 3);
		}
		break;
	}

	return NT_CONTINUE;
}

static t_result handle_enter(t_buffer *buf, char **line)
{
	write(STDOUT_FILENO, "\n", 1);

	*line = nt_buffer_take(buf);
	if (*line == NULL)
		return NT_ERROR;

	return NT_DONE;
}

char *nt_readline(const char *prompt)
{
	unsigned char c;
	ssize_t       n;
	t_buffer      buf;
	char         *line;
	t_result      res;

	line = NULL;

	if (nt_term_enable_raw() == -1) {
		printf("%s", prompt);
		return NULL;
	}

	printf("%s", prompt);
	fflush(stdout);

	if (nt_buffer_init(&buf) == -1) {
		nt_term_restore();
		return NULL;
	}

	res = NT_CONTINUE;
	while (res == NT_CONTINUE) {
		n = read(STDIN_FILENO, &c, sizeof(char));
		if (n != 1) {
			res = (n == 0) ? NT_EOF : NT_ERROR;
			break;
		}

		/* Printable characters, see ascii(7) */
		if (c >= ' ' && c <= '~')
			res = handle_printable(&buf, c);
		else if (c == NT_KEY_CTRL_H || c == NT_KEY_DEL)
			res = handle_backspace(&buf);
		else if (c == NT_KEY_CTRL_D)
			res = handle_ctrl_d(&buf);
		else if (c == NT_KEY_ESC)
			res = handle_escape(&buf);
		else if (c == '\n')
			res = handle_enter(&buf, &line);
	}

	nt_term_restore();
	nt_buffer_free(&buf);
	return line;
}
