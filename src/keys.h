#ifndef NT_KEYS_H
#define NT_KEYS_H

#include "line_buffer.h"
#include "readline_priv.h"

/**
 * @brief Handle one input byte.
 * @param c    Byte read from the input.
 * @param buf  Line being edited.
 * @param nav  Navigation state of the current call.
 * @param line Receives the finished line when NT_DONE is returned.
 * @return NT_CONTINUE to keep reading, NT_DONE when *line is ready (the
 *         caller owns it), NT_EOF on Ctrl+D, NT_ERROR on failure.
 */
t_result nt_key_dispatch(unsigned char c, t_line_buffer *buf, t_nav *nav,
                         char **line);

#endif
