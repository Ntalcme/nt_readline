#ifndef NT_KEYS_H
#define NT_KEYS_H

#include "line_buffer.h"
#include "readline_priv.h"

t_result nt_key_dispatch(unsigned char c, t_line_buffer *buf, t_nav *nav,
                         char **line);

#endif
