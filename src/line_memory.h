#ifndef NT_LINE_MEMORY_H
#define NT_LINE_MEMORY_H

#include <stddef.h>

/** @brief Stored lines. The structure owns every string in lines[]. */
typedef struct s_line_memory {
	char **lines;   /**< stored strings */
	size_t n_lines; /**< number of stored strings */
	size_t cap;     /**< allocated slots in lines[] */
} t_line_memory;

/**
 * @brief Allocate an empty memory.
 * @param mem Memory to initialize.
 * @return 0, or -1 on allocation failure.
 */
int nt_line_memory_init(t_line_memory *mem);

/**
 * @brief Store a copy of @p line.
 * @param mem  Memory to modify.
 * @param line Line to copy; the caller keeps ownership.
 * @return 0 (also when an empty line or a line identical to the last entry
 *         is skipped), or -1 on allocation failure.
 */
int nt_line_memory_add(t_line_memory *mem, const char *line);

/**
 * @brief Get entry @p i.
 * @param mem Memory to read.
 * @param i   Index of the entry.
 * @return The entry, owned by @p mem (do not free), or NULL if out of range.
 */
const char *nt_line_memory_get(const t_line_memory *mem, size_t i);

/**
 * @brief Free every stored line and the array.
 * @param mem Memory to release; safe to call twice.
 */
void nt_line_memory_free(t_line_memory *mem);

#endif
