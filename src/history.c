#include "history.h"
#include "display.h"
#include "line_memory.h"
#include <stdlib.h>
#include <string.h>

static t_line_memory lines_memory;
static int           lines_memory_ready;

/** @brief Return the history, initializing it on first use. */
static t_line_memory *get_memory(void)
{
	if (!lines_memory_ready) {
		if (nt_line_memory_init(&lines_memory) == -1)
			return NULL;
		lines_memory_ready = 1;
	}
	return &lines_memory;
}

/** @brief Return a malloc'd '\0'-terminated copy of buf, or NULL. */
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

/** @brief Replace buf with str and redraw the line. */
static t_result replace_line(t_line_buffer *buf, const char *str)
{
	size_t old;

	old = buf->cursor_pos;
	if (nt_line_buffer_set(buf, str) == -1)
		return NT_ERROR;

	nt_display_replaced(buf, old);
	return NT_CONTINUE;
}

void nt_history_start(t_nav *nav)
{
	t_line_memory *mem;

	mem        = get_memory();
	nav->pos   = (mem != NULL) ? mem->n_lines : 0;
	nav->draft = NULL;
}

void nt_history_end(t_nav *nav)
{
	free(nav->draft);
	nav->draft = NULL;
}

void nt_history_add(const char *line)
{
	t_line_memory *mem;

	/* A history failure must not lose the user's line: ignore it */
	mem = get_memory();
	if (mem != NULL)
		nt_line_memory_add(mem, line);
}

t_result nt_history_move(t_line_buffer *buf, t_nav *nav, t_hist_dir dir)
{
	t_line_memory *mem;
	const char    *str;

	mem = get_memory();
	if (mem == NULL)
		return NT_CONTINUE;

	if (dir == NT_HIST_PREV) {
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

void nt_history_free(void)
{
	if (!lines_memory_ready)
		return;

	nt_line_memory_free(&lines_memory);
	lines_memory_ready = 0;
}
