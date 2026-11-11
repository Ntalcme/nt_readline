#ifndef NT_HISTORY_H
#define NT_HISTORY_H

#include "line_buffer.h"
#include "readline_priv.h"

/** @brief Direction of a history move. */
typedef enum e_hist_dir {
	NT_HIST_PREV, /**< towards older entries */
	NT_HIST_NEXT, /**< towards newer entries */
} t_hist_dir;

/**
 * @brief Init the per-call navigation state.
 * @param nav State to initialize.
 */
void nt_history_start(t_nav *nav);

/**
 * @brief Release the per-call navigation state.
 * @param nav State to release.
 */
void nt_history_end(t_nav *nav);

/**
 * @brief Store a copy of @p line; empty lines and duplicates are skipped.
 * @param line Line to store; the caller keeps ownership.
 */
void nt_history_add(const char *line);

/**
 * @brief Load the previous/next entry into @p buf.
 * @param buf Buffer receiving the entry.
 * @param nav Navigation state of the current call.
 * @param dir Direction of the move.
 * @return NT_ERROR on allocation failure, NT_CONTINUE otherwise.
 */
t_result nt_history_move(t_line_buffer *buf, t_nav *nav, t_hist_dir dir);

/** @brief Free every stored line. Safe to call twice. */
void nt_history_free(void);

#endif
