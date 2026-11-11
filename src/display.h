#ifndef NT_DISPLAY_H
#define NT_DISPLAY_H

#include "line_buffer.h"
#include <stddef.h>

void nt_display_cursor_left(size_t n);
void nt_display_cursor_right(size_t n);
void nt_display_newline(void);

/* To call after a char was inserted in buf */
void nt_display_inserted(const t_line_buffer *buf);

/* To call after the char before the cursor was deleted from buf */
void nt_display_deleted(const t_line_buffer *buf);

/* To call after buf content was replaced; old_cursor is the cursor
 * position before the replacement */
void nt_display_replaced(const t_line_buffer *buf, size_t old_cursor);

#endif
