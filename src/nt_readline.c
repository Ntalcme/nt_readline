#include "nt_readline.h"
#include "line_buffer.h"
#include "line_memory.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

typedef struct s_nav {
	size_t pos;   /* index in history; n_lines means "line being typed" */
	char  *draft; /* copy of the line being typed, saved on first up */
} t_nav;

static t_line_memory lines_memory;
static int           lines_memory_ready;

static t_line_memory *get_memory(void)
{
	if (!lines_memory_ready) {
		if (nt_line_memory_init(&lines_memory) == -1)
			return NULL;
		lines_memory_ready = 1;
	}
	return &lines_memory;
}

static char *dup_buffer(const t_line_buffer *buf)
{
	char *copy;

	copy = malloc(buf->len + 1);
	if (copy == NULL)
		return NULL;

	memcpy(copy, buf->data, buf->len);
	copy[buf->len] = '\0';
	return copy;
}

static t_result replace_line(t_line_buffer *buf, const char *str)
{
	size_t old;
	size_t i;

	old = buf->cursor_pos;
	if (nt_line_buffer_set(buf, str) == -1)
		return NT_ERROR;

	/* Go back to the start of the line, erase it, draw the new one */
	for (i = 0; i < old; i++)
		write(STDOUT_FILENO, "\033[D", 3);
	write(STDOUT_FILENO, "\033[K", 3);
	write(STDOUT_FILENO, buf->data, buf->len);

	return NT_CONTINUE;
}

static t_result history_move(t_line_buffer *buf, t_nav *nav, int up)
{
	t_line_memory *mem;
	const char    *str;

	mem = get_memory();
	if (mem == NULL)
		return NT_CONTINUE;

	if (up) {
		if (nav->pos == 0)
			return NT_CONTINUE;
		if (nav->pos == mem->n_lines) {
			free(nav->draft);
			nav->draft = dup_buffer(buf);
			if (nav->draft == NULL)
				return NT_ERROR;
		}
		nav->pos--;
	} else {
		if (nav->pos >= mem->n_lines)
			return NT_CONTINUE;
		nav->pos++;
	}

	if (nav->pos == mem->n_lines)
		str = nav->draft;
	else
		str = nt_line_memory_get(mem, nav->pos);
	if (str == NULL)
		return NT_CONTINUE;

	return replace_line(buf, str);
}

static t_result handle_printable(t_line_buffer *buf, unsigned char c)
{
	size_t tail;
	size_t i;

	if (nt_line_buffer_insert(buf, c) == -1)
		return NT_ERROR;

	/* Redraw the inserted char and everything after it */
	tail = buf->len - buf->cursor_pos;
	write(STDOUT_FILENO, buf->data + buf->cursor_pos - 1, tail + 1);

	/* Move the cursor back, just after the inserted char */
	for (i = 0; i < tail; i++)
		write(STDOUT_FILENO, "\033[D", 3);

	return NT_CONTINUE;
}

static t_result handle_backspace(t_line_buffer *buf)
{
	size_t tail;
	size_t i;

	if (buf->cursor_pos == 0)
		return NT_CONTINUE;

	nt_line_buffer_delete(buf);
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

static t_result handle_ctrl_d(const t_line_buffer *buf)
{
	/* Ctrl+D on a non-empty line is ignored */
	if (buf->len > 0)
		return NT_CONTINUE;

	write(STDOUT_FILENO, "\n", 1);
	return NT_EOF;
}

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
	case 'A': /* up */
		res = history_move(buf, nav, 1);
		break;

	case 'B': /* down */
		res = history_move(buf, nav, 0);
		break;

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

	return res;
}

static t_result handle_enter(t_line_buffer *buf, char **line)
{
	t_line_memory *mem;

	write(STDOUT_FILENO, "\n", 1);

	*line = nt_line_buffer_take(buf);
	if (*line == NULL)
		return NT_ERROR;

	/* A history failure must not lose the user's line: ignore it */
	mem = get_memory();
	if (mem != NULL)
		nt_line_memory_add(mem, *line);

	return NT_DONE;
}

void nt_readline_free_history(void)
{
	if (!lines_memory_ready)
		return;

	nt_line_memory_free(&lines_memory);
	lines_memory_ready = 0;
}

char *nt_readline(const char *prompt)
{
	unsigned char  c;
	ssize_t        n;
	t_line_buffer  buf;
	char          *line;
	t_result       res;
	t_nav          nav;
	t_line_memory *mem;

	line = NULL;

	if (nt_term_enable_raw() == -1) {
		printf("%s", prompt);
		return NULL;
	}

	printf("%s", prompt);
	fflush(stdout);

	if (nt_line_buffer_init(&buf) == -1) {
		nt_term_restore();
		return NULL;
	}

	mem       = get_memory();
	nav.pos   = (mem != NULL) ? mem->n_lines : 0;
	nav.draft = NULL;
	res       = NT_CONTINUE;
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
			res = handle_escape(&buf, &nav);
		else if (c == '\n')
			res = handle_enter(&buf, &line);
	}

	nt_term_restore();
	nt_line_buffer_free(&buf);
	free(nav.draft);

	if (res != NT_DONE) {
		free(line);
		line = NULL;
	}
	return line;
}
