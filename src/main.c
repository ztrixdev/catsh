#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "config/catsh_conf.h"
#include "terminal/terminal.h"
#include "journal/journal.h"
#include "tokenizer/tokenizer.h"
#include "env/env.h"

bool terminated = 0;

void process_line(charvec* journal, char* line);

int main(int argc, char** argv)
{
    /*
     * read and load ~/.catsh_config and ~/.catsh_journal
     */
    catsh_config cfg;
    if (catsh_config_file_exists()) cfg = load_catsh_config();
    else
    {
        init_catsh_config();
        load_catsh_config();
    }

    charvec* journal;
    if (journal_file_exists()) journal = read_from_journal();
    else
    {
        init_journal_file();
        journal = read_from_journal();
    }

    size_t journal_position = charvec_length(journal);

    enable_raw_mode();

    char *scratch_buffer = malloc(2048);

    /*
     * main shell loop
     */
    while(!terminated)
    {
        printf("\r\x1b[K%s > ", cfg.cat);
        fflush(stdout);

        char *line = malloc(2048);
        size_t line_idx = 0;

        while(1)
        {
            // catching a key
            int key = read_key();

            // catch Return
            if (key == 10 || key == 13)
            {
                // null term the string
                line[line_idx] = '\0';
                printf("\r\n");
                fflush(stdout);
                process_line(journal, line);
                journal_position = charvec_length(journal); // reset the position in history
                break;
            }

            // catch backspace and ctrl+h
            if (key == 127 || key == 8)
            {
                if (line_idx > 0)
                {
                    line_idx--;
                    line[line_idx] = '\0';
                    printf("\b \b");
                    fflush(stdout);
                }
                continue;
            }

            // catch arrows
            if (key == 1000 || key == 1001)
            {
                if (journal_position == charvec_length(journal)) {
                    line[line_idx] = '\0';
                    strcpy(scratch_buffer, line);
                }
                printf("\r\x1b[K%s > ", cfg.cat);

                if (key == 1000 && journal_position > 0)
                {
                    journal_position--;
                }
                else if (key == 1001 && journal_position < charvec_length(journal))
                {
                    journal_position++;
                }

                // retrieve line text from history or scratch buffer

                // if we reach the bottom line of the journal we restore from the scratch buffer
                if (journal_position == charvec_length(journal))
                {
                    strcpy(line, scratch_buffer);
                }
                else
                {
                    char *history_line = charvec_get(journal, journal_position);
                    if (history_line != NULL) {
                        strncpy(line, history_line, 2047);
                        line[2047] = '\0';
                    } else {
                        line[0] = '\0';
                    }
                }

                // update index counter and print the newly populated line string
                line_idx = strlen(line);
                printf("%s", line);
                fflush(stdout);
                continue;
            }

            // catch regular chars
            if (line_idx < 2047)
            {
                line[line_idx++] = key;
                printf("%c", key);
                fflush(stdout);
            }
        }
    }

    return 0;
}


void process_line(charvec* journal, char* line)
{
    if (strlen(line) == 0)
        return;

    // strip away the newlines if they're there
    line[strcspn(line, "\n")] = '\0';
    line[strcspn(line, "\r")] = '\0';

    append_to_journal(journal, line);

    charvec* tokens = tokenize(line,strlen(line));
    if (strcmp(charvec_get(tokens, 0), "exit") == 0)
    {
        puts("it was nice meowing with you!");
        terminated = true;
        return;
    }

    char** ptr = charvec_get_nterm_dataptr(tokens);
    pid_t pid = fork();
    if (pid == 0)
    {
        execvp(charvec_get(tokens, 0), ptr);
        execv(charvec_get(tokens, 0), ptr);

        perror("catsh execution failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        waitpid(pid, NULL, 0); // wait for the child process to exit
    }
    else
    {
        perror("error in fork call");
    }

    free(line);
}
