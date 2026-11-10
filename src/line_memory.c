#include "line_memory.h"
#include <stdlib.h>

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
