#include <stddef.h>

#include "charvec.h"

#ifndef TOKENIZER_H
#define TOKENIZER_H

charvec** parse_commands(char* line, size_t length, size_t* n_cmd);
void free_commands(charvec** commands);

#endif
