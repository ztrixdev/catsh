#include "tokenizer.h"
#include <stdio.h>
#include <ctype.h>

charvec* tokenize(char *line, size_t length)
{
    charvec* tokens = charvec_init();

    char* current_token = malloc(128*sizeof(char));
    int current_token_idx = 0;
    for (size_t idx = 0; idx <= length; idx++)
    {
        if (line[idx] == ' ' || line[idx] == '\0')
        {
            if (current_token_idx != 0) // only append a new token if it exists
            {
                current_token[current_token_idx] = '\0';
                charvec_append(tokens, current_token);
                current_token = malloc(128 * sizeof(char));
                current_token_idx = 0;
            }
        }
        else
        {
            current_token[current_token_idx] = line[idx];
            current_token_idx++;
        }
    }

    return tokens;
}
