#include "nt_readline.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *line;

	while ((line = nt_readline("> ")) != NULL) {
		printf("you typed: %s\n", line);
		free(line);
	}
	return 0;
}
