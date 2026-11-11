#ifndef TERMINAL_H
#define TERMINAL_H

/**
 * @brief Switch the terminal to non-canonical mode without echo.
 *
 * The previous settings are saved.
 *
 * @return 0, or -1 if stdin is not a terminal or the settings cannot be
 *         applied.
 */
int nt_term_enable_raw(void);

/**
 * @brief Restore the saved settings.
 *
 * Call only after nt_term_enable_raw() succeeded.
 */
void nt_term_restore(void);

#endif
