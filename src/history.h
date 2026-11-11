#ifndef NT_HISTORY_H
#define NT_HISTORY_H

#include "line_buffer.h"
#include "readline_priv.h"

typedef enum e_hist_dir {
	NT_HIST_PREV,
	NT_HIST_NEXT,
} t_hist_dir;

void     nt_history_start(t_nav *nav);
void     nt_history_end(t_nav *nav);
void     nt_history_add(const char *line);
t_result nt_history_move(t_line_buffer *buf, t_nav *nav, t_hist_dir dir);
void     nt_history_free(void);

#endif
