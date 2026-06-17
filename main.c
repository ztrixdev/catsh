#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "conf/catsh_conf.h"
#include "tokenizer/tokenizer.h"
#include "env/env.h"

void execute(char *line, charvec *path);

bool terminated = false;

int main(int argc, char **argv)
{
    charvec* PATH = load_path();
    catsh_config cnf = load_catsh_config();

    chdir(cnf.default_working_dir);
    while (!terminated)
    {
        printf("%s > ", cnf.cat);

        char *line = malloc(256 * sizeof(char));
        fgets(line, 256 * sizeof(char), stdin);
        line[strcspn(line, "\n")] = '\0';
        execute(line, PATH);
        free(line);
        continue;
    }

    return 0;
}

void execute(char *line, charvec *path)
{
    charvec* tokens = tokenize(line, strlen(line));

    if (strcmp(charvec_get(tokens, 0), "exit") == 0)
    {
        puts("it was nice meowing with you!");
        terminated = true;
    }

    char** ptr = charvec_get_nterm_dataptr(tokens);
    pid_t pid = fork();
    if (pid == 0)
    {
        // try to execute from the PATH
        if (execvp(charvec_get(tokens, 0), ptr) == -1) {
            execv(charvec_get(tokens, 0), ptr); // execute directly on fail
        }
    }
    else if (pid > 0) waitpid(pid, NULL, 0);
    else perror("error in fork call");

}
