#include "line_memory.h"
#include <stdlib.h>
#include <string.h>

#define NT_LINE_MEMORY_INIT_CAP 16

int nt_line_memory_init(t_line_memory *mem)
{
	char **lines;
	size_t data_cap;

	data_cap = NT_LINE_MEMORY_INIT_CAP;
	lines    = malloc(data_cap * sizeof(char *));
	if (lines == NULL)
		return -1;

	mem->cap     = data_cap;
	mem->lines   = lines;
	mem->n_lines = 0;

	return 0;
}

int nt_line_memory_add(t_line_memory *mem, const char *line)
{
	char **new_lines;
	char  *copy;
	size_t new_cap;

	/* Ignore empty lines and consecutive duplicates */
	if (line[0] == '\0')
		return 0;
	if (mem->n_lines > 0 && strcmp(mem->lines[mem->n_lines - 1], line) == 0)
		return 0;

	if (mem->n_lines >= mem->cap) {
		new_cap   = mem->cap * 2;
		new_lines = realloc(mem->lines, new_cap * sizeof(char *));
		if (new_lines == NULL)
			return -1;
		mem->lines = new_lines;
		mem->cap   = new_cap;
	}

	copy = strdup(line);
	if (copy == NULL)
		return -1;

	mem->lines[mem->n_lines] = copy;
	mem->n_lines++;
	return 0;
}

const char *nt_line_memory_get(const t_line_memory *mem, size_t i)
{
	if (i >= mem->n_lines)
		return NULL;

	return mem->lines[i];
}

void nt_line_memory_free(t_line_memory *mem)
{
	size_t i;

	for (i = 0; i < mem->n_lines; i++)
		free(mem->lines[i]);

	free(mem->lines);
	mem->lines   = NULL;
	mem->n_lines = 0;
	mem->cap     = 0;
}
