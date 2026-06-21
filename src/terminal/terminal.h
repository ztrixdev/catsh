#include <termios.h>

#ifndef TERMINAL_H
#define TERMINAL_H

void enable_raw_mode(void);
void disable_raw_mode(void);
int read_key(void);

#endif
