#include <stdio.h>
#include <ctype.h>

#include "tokenizer.h"
#include "str_consts.h"

charvec* tokenize(char* line, size_t length)
{
    charvec* tokens = charvec_init();

    char* current_token = malloc(128*sizeof(char));
    int current_token_idx = 0;
    for (size_t idx = 0; idx <= length; idx++)
    {
        if (line[idx] == ' ' ||
            (line[idx] == '\0' &&
             (idx == 0 || line[idx-1] != '\\')))
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

charvec** parse_commands(char* line, size_t length, size_t* n_cmd)
{
    charvec** commands = malloc(4096 * sizeof(charvec *));
    size_t cmd_cv_idx = 0;

    char* current_command = malloc(256*sizeof(char));
    size_t current_command_idx = 0;
    for (size_t idx = 0; idx <= length; idx++)
    {
        switch (line[idx])
        {
            case '|':
                commands[cmd_cv_idx] = tokenize(current_command, current_command_idx);
                cmd_cv_idx++;
                current_command = malloc(256*sizeof(char));
                current_command_idx = 0;
                commands[cmd_cv_idx] = charvec_init();
                charvec_append(commands[cmd_cv_idx], PIPE_TO_STR); // pipe output to (|)
                cmd_cv_idx++;
                break;
            case '>':
                commands[cmd_cv_idx] = tokenize(current_command, current_command_idx);
                cmd_cv_idx++;
                current_command = malloc(256*sizeof(char));
                current_command_idx = 0;
                commands[cmd_cv_idx] = charvec_init();
                if (idx+1 <= length && line[idx+1] == '>')
                {
                    charvec_append(commands[cmd_cv_idx], REDIRECT_APPEND_STR); // redirect to file appending to it (>>)
                    idx++;
                }
                else charvec_append(commands[cmd_cv_idx], REDIRECT_OVERWRITE_STR); // redirect to file overwriting it (>)
                cmd_cv_idx++;
                break;
            default:
                current_command[current_command_idx] = line[idx];
                current_command_idx++;
                break;
        }
    }
    if (current_command_idx > 0)
        commands[cmd_cv_idx++] = tokenize(current_command, current_command_idx);  // add trailing command

    commands[cmd_cv_idx] = NULL;
    *n_cmd = cmd_cv_idx;
    return commands;
}

void free_commands(charvec **commands)
{
    if (!commands)
        return;

    for (size_t idx = 0; commands[idx] != NULL; idx++)
    {
        charvec_free(commands[idx]);
    }

    free(commands);
}
