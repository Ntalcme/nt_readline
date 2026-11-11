#include "nt_readline.h"
#include "history.h"
#include "keys.h"
#include "line_buffer.h"
#include "readline_priv.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void nt_readline_free_history(void)
{
	nt_history_free();
}

char *nt_readline(const char *prompt)
{
	unsigned char c;
	ssize_t       n;
	t_line_buffer buf;
	char         *line;
	t_result      res;
	t_nav         nav;

	line = NULL;

	if (nt_term_enable_raw() == -1) {
		printf("%s", prompt);
		return NULL;
	}

	printf("%s", prompt);
	fflush(stdout);

	if (nt_line_buffer_init(&buf) == -1) {
		nt_term_restore();
		return NULL;
	}

	nt_history_start(&nav);
	res = NT_CONTINUE;
	while (res == NT_CONTINUE) {
		n = read(STDIN_FILENO, &c, sizeof(char));
		if (n != 1) {
			res = (n == 0) ? NT_EOF : NT_ERROR;
			break;
		}
		res = nt_key_dispatch(c, &buf, &nav, &line);
	}

	nt_term_restore();
	nt_line_buffer_free(&buf);
	nt_history_end(&nav);

	if (res != NT_DONE) {
		free(line);
		line = NULL;
	}
	return line;
}
