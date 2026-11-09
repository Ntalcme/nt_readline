#include "terminal.h"
#include <termios.h>
#include <unistd.h>

static struct termios g_orig;

int nt_term_enable_raw(void)
{
	struct termios raw;

	if (tcgetattr(STDIN_FILENO, &g_orig) == -1)
		return -1;

	raw = g_orig;
	raw.c_lflag &= ~(ECHO | ICANON);
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		return -1;

	return 0;
}

void nt_term_restore(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_orig);
}
