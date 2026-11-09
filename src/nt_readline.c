#include "nt_readline.h"
#include "terminal.h"
#include <stdio.h>
#include <unistd.h>

char *nt_readline(const char *prompt)
{
	unsigned char c;
	ssize_t	      n;

	if (nt_term_enable_raw() == -1) {
		printf("%s", prompt);
		return NULL;
	}

	printf("%s", prompt);
	fflush(stdout);

	while (1) {
		n = read(STDIN_FILENO, &c, sizeof(char));
		if (n != 1)
			break;
		/* Printable characters, see ascii(7) */
		if (c >= 32 && c <= 126)
			write(STDOUT_FILENO, &c, 1);
		if (c == '\n') {
			write(STDOUT_FILENO, "\n", 1);
			break;
		}
	}

	nt_term_restore();
	return NULL;
}
