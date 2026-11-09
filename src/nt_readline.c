#include "nt_readline.h"
#include "buffer.h"
#include "terminal.h"
#include <stdio.h>
#include <unistd.h>

char *nt_readline(const char *prompt)
{
	unsigned char c;
	ssize_t	      n;
	t_buffer      buf;
	char	     *line;

	line = NULL;

	if (nt_term_enable_raw() == -1) {
		printf("%s", prompt);
		return NULL;
	}

	printf("%s", prompt);
	fflush(stdout);

	if (nt_buffer_init(&buf) == -1) {
		nt_term_restore();
		return NULL;
	}

	while (1) {
		n = read(STDIN_FILENO, &c, sizeof(char));
		if (n != 1)
			break;

		/* Printable characters, see ascii(7) */
		if (c >= 32 && c <= 126) {
			if (nt_buffer_push(&buf, c) == -1)
				break;
			write(STDOUT_FILENO, &c, 1);
		} else if (c == 8 || c == 127) {
			if (buf.len > 0) {
				nt_buffer_pop(&buf);
				write(STDOUT_FILENO, "\b \b", 3);
			}
		} else if (c == 4 && buf.len == 0) {
			write(STDOUT_FILENO, "\n", 1);
			break;
		}

		if (c == '\n') {
			write(STDOUT_FILENO, "\n", 1);
			line = nt_buffer_take(&buf);
			break;
		}
	}

	nt_term_restore();
	nt_buffer_free(&buf);
	return line;
}
