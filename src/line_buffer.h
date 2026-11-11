#ifndef NT_LINE_BUFFER_H
#define NT_LINE_BUFFER_H

#include <stddef.h>

/** @brief Editable line. Invariant: cursor_pos <= len < cap. */
typedef struct s_line_buffer {
	char  *data;       /**< bytes, without a trailing '\0' */
	size_t len;        /**< number of bytes used */
	size_t cap;        /**< allocated size */
	size_t cursor_pos; /**< cursor index in [0, len] */
} t_line_buffer;

/**
 * @brief Allocate an empty buffer.
 * @param buf Buffer to initialize.
 * @return 0, or -1 on allocation failure.
 */
int nt_line_buffer_init(t_line_buffer *buf);

/**
 * @brief Insert @p c at cursor_pos and advance the cursor.
 * @param buf Buffer to modify.
 * @param c   Char to insert.
 * @return 0, or -1 on failure.
 */
int nt_line_buffer_insert(t_line_buffer *buf, const char c);

/**
 * @brief Delete the char before the cursor and move the cursor back.
 * @param buf Buffer to modify; nothing happens if cursor_pos is 0.
 */
void nt_line_buffer_delete(t_line_buffer *buf);

/**
 * @brief Replace the whole content with @p str; cursor goes to the end.
 * @param buf Buffer to modify.
 * @param str '\0'-terminated string to copy.
 * @return 0, or -1 on failure.
 */
int nt_line_buffer_set(t_line_buffer *buf, const char *str);

/**
 * @brief Take the content out of the buffer.
 * @param buf Buffer to empty; unusable until it is initialized again.
 * @return The content as a '\0'-terminated string; the caller owns it and
 *         must free() it.
 */
char *nt_line_buffer_take(t_line_buffer *buf);

/**
 * @brief Release the memory.
 * @param buf Buffer to release; safe after nt_line_buffer_take() or twice.
 */
void nt_line_buffer_free(t_line_buffer *buf);

#endif
