#ifndef NT_READLINE_PRIV_H
#define NT_READLINE_PRIV_H

#include <stddef.h>

/** @brief Outcome of handling one input byte. */
typedef enum e_result {
	NT_CONTINUE, /**< keep reading */
	NT_DONE,     /**< Enter pressed, line is ready */
	NT_EOF,      /**< Ctrl+D on an empty line, or input closed */
	NT_ERROR,    /**< read or allocation failure */
} t_result;

/** @brief History navigation state of one nt_readline() call. */
typedef struct s_nav {
	size_t pos;   /**< index in history; n_lines means "line being typed" */
	char  *draft; /**< copy of the line being typed, saved on first up */
} t_nav;

#endif
