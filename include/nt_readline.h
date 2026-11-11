#ifndef NT_READLINE_H
#define NT_READLINE_H

/**
 * @brief Read one edited line from the terminal.
 * @param prompt String printed before reading.
 * @return A malloc'd line to free(), or NULL on Ctrl+D (empty line) or error.
 */
char *nt_readline(const char *prompt);

/** @brief Release the history; call once before exiting. Safe to call twice. */
void nt_readline_free_history(void);

#endif
