#include <termios.h>
#include <unistd.h>

#include "terminal.h"

struct termios orig_termios;

void enable_raw_mode(void)
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;

    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int read_key(void) {
    char c;
    int nread;

    // reading a single byte
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1) return -1;
    }

    // catching escape sequences
    if (c == '\x1b') {
        char seq[2];

        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return 1000; // arr up
                case 'B': return 1001; // arr down
                case 'C': return 1002; // arr left
                case 'D': return 1003; // arr right
            }
        }
        return '\x1b'; // it was just ESC
    }

    return c;
}
