#ifndef NT_DISPLAY_H
#define NT_DISPLAY_H

#include "line_buffer.h"
#include <stddef.h>

/**
 * @brief Move the screen cursor left by @p n columns.
 * @param n Number of columns; the buffer is not modified.
 */
void nt_display_cursor_left(size_t n);

/**
 * @brief Move the screen cursor right by @p n columns.
 * @param n Number of columns; the buffer is not modified.
 */
void nt_display_cursor_right(size_t n);

/** @brief Print a newline. */
void nt_display_newline(void);

/**
 * @brief Redraw after nt_line_buffer_insert().
 * @param buf Buffer in which a char was just inserted.
 */
void nt_display_inserted(const t_line_buffer *buf);

/**
 * @brief Redraw after nt_line_buffer_delete().
 * @param buf Buffer from which a char was just deleted.
 */
void nt_display_deleted(const t_line_buffer *buf);

/**
 * @brief Redraw the whole line after its content was replaced.
 * @param buf        Buffer holding the new content.
 * @param old_cursor Cursor position before the replacement.
 */
void nt_display_replaced(const t_line_buffer *buf, size_t old_cursor);

#endif
