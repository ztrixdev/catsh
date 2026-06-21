#include <stdbool.h>

#ifndef CATSH_CONF_H
#define CATSH_CONF_H

#define CONFIG_FILENAME ".catsh_config"

typedef struct
{
    char* cat;
    char* default_working_dir;
}
catsh_config;

bool catsh_config_file_exists(void);
char* get_catsh_config_path(void);
void init_catsh_config(void);
catsh_config load_catsh_config(void);

#endif
