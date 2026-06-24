#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "catsh_conf.h"
#include "env.h"


char* get_catsh_config_path(void)
{
    char *home = get_home();
    if (home == NULL)
        return NULL;

    char *path = malloc(PATH_MAX);
    snprintf(path, PATH_MAX, "%s/%s", home, CONFIG_FILENAME);

    return path;
}

bool catsh_config_file_exists(void)
{
    char *path = get_catsh_config_path();
    return access(path, F_OK) == 0;
}

void init_catsh_config(void)
{
    char *home = get_home();
    char *path = get_catsh_config_path();
    if (path == NULL || home == NULL)
    {
        printf("could not initialize catsh config due to path issues! exiting... \n");
        exit(1);
    }

    FILE *file_ptr = fopen(path, "w");
    if (file_ptr == NULL)
    {
        printf("could not initialize catsh config due to file i/o issues! exiting... \n");
        perror("fopen");
        exit(1);
    }

    int res = fprintf(file_ptr, "cat=\"ᓚᘏᗢ\"\ndefault_working_dir=\"%s\"\n", home);
    if (res < 0)
    {
        printf("could not write to catsh config! exiting... \n");
        perror("fprintf");
        exit(1);
    }

    fclose(file_ptr);
    return;
}

catsh_config load_catsh_config(void)
{
    char *path = get_catsh_config_path();
    if (path == NULL)
    {
        printf("could not read catsh config due to path issues! exiting... \n");
        exit(1);
    }

    catsh_config conf;
    FILE *file_ptr = fopen(path, "r");
    if (file_ptr == NULL)
    {
        printf("could not read catsh config due to file i/o issues! exiting... \n");
        perror("fopen");
        exit(1);
    }

    char line[512];
    while(fgets(line, 512, file_ptr)) {
        if (line[0] == '!') // comments start with !
            continue;
        if (strncmp(line,"cat=\"", 5) == 0)
        {
            conf.cat = malloc(32);
            size_t idx = 5;
            while (line[idx] != '"')
            {
                conf.cat[idx-5] = line[idx];
                idx++;
            }
        }
        if (strncmp(line, "default_working_dir=\"", 21) == 0)
        {
            conf.default_working_dir = malloc(PATH_MAX);
            size_t idx = 21;
            while (line[idx] != '"')
            {
                conf.default_working_dir[idx-21] = line[idx];
                idx++;
            }
        }
    }

    return conf;
}
