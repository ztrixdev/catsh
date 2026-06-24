#include <string.h>
#include <limits.h>
#include <stdio.h>

#include "charvec.h"
#include "env.h"

charvec* load_path(void)
{
    charvec *path_vector = charvec_init();

    char *path = getenv("PATH");
    size_t path_len = strlen(path);
    char *current_entry =  malloc(PATH_MAX);
    size_t current_entry_idx = 0;
    for (size_t idx = 0; idx < path_len; idx++)
    {
        if (path[idx] == ':')
        {
            if (current_entry_idx != 0)
            {
                current_entry[current_entry_idx] = '\0';
                charvec_append(path_vector, current_entry);
                current_entry = malloc(PATH_MAX);
                current_entry_idx = 0;
            }
        }
        else
        {
            current_entry[current_entry_idx] = path[idx];
            current_entry_idx++;
        }
    }

    return path_vector;
}

char* get_home(void)
{
    char* home = getenv("HOME");
    if (home == NULL)
        home = getenv("USERPROFILE");
    if (home == NULL)
    {
        printf("Neither $HOME or %%USERPROFILE%% are set.\n");
        return NULL;
    }

    return home;
}
