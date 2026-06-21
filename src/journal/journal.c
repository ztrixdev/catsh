#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>

#include "journal.h"
#include "../env/env.h"


char* get_journal_path(void)
{
    char *home = get_home();
    if (home == NULL)
        return NULL;

    char *path = malloc(PATH_MAX);
    snprintf(path, PATH_MAX, "%s/%s", home, JOURNAL_FILENAME);

    return path;
}

bool journal_file_exists(void)
{
    char *path = get_journal_path();
    return access(path, F_OK) == 0;
}

FILE* get_journal_file_ptr(char* mode)
{
    char *path = get_journal_path();
    FILE *jptr = fopen(path, mode);

    if (jptr == NULL)
    {
        printf("could not open catsh journal due to file i/o issues! exiting... \n");
        perror("fopen");
        exit(1);
    }

    return jptr;
}

void init_journal_file(void)
{
    char *home = get_home();
    char *path = get_journal_path();
    if (path == NULL || home == NULL)
    {
        printf("could not initialize catsh journal due to path issues! exiting... \n");
        exit(1);
    }

    FILE* file_ptr = get_journal_file_ptr("w");

    fclose(file_ptr);
    return;
}

void clear_journal(charvec* jcv)
{
    free(jcv);
    jcv = charvec_init();

    FILE* jptr = get_journal_file_ptr("wt");

    fprintf(jptr, "");
    fclose(jptr);

    return;
}

charvec* read_from_journal(void)
{
    charvec *journal = charvec_init();

    FILE *jptr = get_journal_file_ptr("r");

    // init buffers
    char *line_buffer = NULL;
    size_t line_capacity = 0;
    ssize_t line_length;

    while ((line_length = getline(&line_buffer, &line_capacity, jptr)) != -1) {
        // replace \n with \0 to null-terminate
        if (line_length > 0 && line_buffer[line_length - 1] == '\n') {
            line_buffer[line_length - 1] = '\0';
        }

        // add unique heap ptr to charvec
        charvec_append(journal, line_buffer);

        // reset heap ptrs
        line_buffer = NULL;
        line_capacity = 0;
    }

    fclose(jptr);
    return journal;
}

void append_to_journal(charvec* jcv, char* entry)
{
    charvec_append(jcv, entry);

    FILE* jptr = get_journal_file_ptr("at");

    fprintf(jptr, "%s\n", entry);
    fclose(jptr);
}
