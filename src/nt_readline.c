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
	if (nt_buffer_push(buf, c) == -1)
		return NT_ERROR;

	write(STDOUT_FILENO, &c, 1);
	return NT_CONTINUE;
}

static t_result handle_backspace(t_buffer *buf)
{
	if (buf->len > 0) {
		nt_buffer_pop(buf);
		write(STDOUT_FILENO, "\b \b", 3);
	}
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

static t_result handle_escape(void)
{
	unsigned char seq[2];

	if (read(STDIN_FILENO, &seq[0], 1) != 1 ||
	    read(STDIN_FILENO, &seq[1], 1) != 1)
		return NT_ERROR;

	/* Arrow keys (ESC [ A/B/C/D) are consumed and ignored for now */
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
			res = handle_escape();
		else if (c == '\n')
			res = handle_enter(&buf, &line);
	}

	nt_term_restore();
	nt_buffer_free(&buf);
	return line;
}
