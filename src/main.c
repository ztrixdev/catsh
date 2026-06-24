#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "catsh_conf.h"
#include "str_consts.h"
#include "terminal.h"
#include "journal.h"
#include "tokenizer.h"
#include "env.h"

bool terminated = 0;

void process_line(charvec* journal, char* line);
void execute_piped(charvec* cmd1, charvec* cmd2);
void execute_standalone(charvec* cmd);

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

    size_t n_cmd = 0;
    charvec** commands = parse_commands(line, strlen(line), &n_cmd);

    if (n_cmd == 0) return;
    else if (n_cmd == 1) execute_standalone(commands[0]);
    else if (n_cmd > 1)
    {
        for (size_t idx = 0; commands[idx] != NULL; idx++)
        {
            char* cmd = charvec_get(commands[idx], 0);
            if (strcmp(cmd, PIPE_TO_STR) == 0)
            {
                if (commands[idx-1] != NULL && commands[idx+1] != NULL
                    && charvec_length(commands[idx-1]) >= 1 && charvec_length(commands[idx+1]) >= 1 )
                {
                    execute_piped(commands[idx-1], commands[idx+1]);
                }
            }
        }
    }

    free_commands(commands);
    free(line);
}


void execute_piped(charvec* cmd1, charvec* cmd2)
{
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t p1 = fork();

    if (p1 == 0) {
        close(fd[0]);

        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        char **args = charvec_get_nterm_dataptr(cmd1);
        execvp(charvec_get(cmd1,0), args);

        perror("execvp");
        exit(EXIT_FAILURE);
    }

    pid_t p2 = fork();
    if (p2 == 0) {
        close(fd[1]);

        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);

        char **args = charvec_get_nterm_dataptr(cmd2);
        execvp(charvec_get(cmd2,0), args);

        perror("execvp");
        exit(EXIT_FAILURE);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
}

void execute_standalone(charvec* cmd)
{
    char** ptr = charvec_get_nterm_dataptr(cmd);
    pid_t pid = fork();
    if (pid == 0)
    {
        execvp(charvec_get(cmd, 0), ptr);

        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        waitpid(pid, NULL, 0);
    }
    else
    {
        perror("error in fork call");
    }
}
