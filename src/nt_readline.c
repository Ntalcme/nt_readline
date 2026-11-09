#include "nt_readline.h"
#include "terminal.h"
#include <stdio.h>
#include <unistd.h>

char *nt_readline(const char *prompt)
{
	unsigned char c;
	ssize_t n;

	if (nt_term_enable_raw() == -1) {
		printf("%s", prompt);
		return NULL;
	}

	printf("%s", prompt);
	fflush(stdout);
	n = read(STDIN_FILENO, &c, sizeof(char));
	nt_term_restore();
	if (n != 1)
		return NULL;

	printf("%d\n", c);
	
	return NULL;
}
