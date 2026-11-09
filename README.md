| Project |                   nt_readline |
| :------ | ----------------------------: |
| Open    | 2026-11-09 CEST (UTC+1)       |
| Closed  |              *Not closed yet* |  
---
**nt_readline** is a custom implementation of the core of the
[GNU Readline](https://tiswww.case.edu/php/chet/readline/rltop.html) library,
written from scratch in C as a learning project. It is **not** a drop-in
replacement: it only aims to reproduce the basic line-editing behavior.

## Usage

```c
#include "nt_readline.h"

char *line = nt_readline("> ");
```

`nt_readline` prints the prompt, reads one line in raw terminal mode and
returns it as a heap-allocated string. The caller must `free` it. It returns
`NULL` on end of input (Ctrl+D on an empty line) or on error.

## Build

```sh
make          # build the static library
make debug    # build with AddressSanitizer
make fclean   # remove all build artifacts
```

## Status

- [x] Raw terminal mode (termios)
- [x] Printable characters and echo
- [x] Dynamic line buffer
- [x] Backspace
- [x] Ctrl+D (EOF)
- [ ] Arrow keys (escape sequences)
- [ ] Cursor movement and insertion in the middle of the line
- [ ] History
- [ ] Ctrl+C handling (restore the terminal)
- [ ] Non-tty input fallback
- [ ] UTF-8

## Requirements

A POSIX system (Linux, macOS, BSD), `gcc` or `clang`, `make`.
Built with `-std=gnu99`.

## License
Released under the [MIT License](LICENSE). 
