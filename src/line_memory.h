#ifndef NT_LINE_MEMORY_H
#define NT_LINE_MEMORY_H

#include <stddef.h>

typedef struct s_line_memory {
	char **lines;
	size_t n_lines;
	size_t cap;
} t_line_memory;

int         nt_line_memory_init(t_line_memory *mem);
int         nt_line_memory_add(t_line_memory *mem, const char *line);
const char *nt_line_memory_get(const t_line_memory *mem, size_t i);
void        nt_line_memory_free(t_line_memory *mem);

#endif
