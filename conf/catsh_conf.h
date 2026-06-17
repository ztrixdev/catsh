#ifndef CATSH_CONF_H
#define CATSH_CONF_H

typedef struct
{
    char* cat;
    char* default_working_dir;
}
catsh_config;

char* get_catsh_config_path(void);
void init_catsh_config(void);
catsh_config load_catsh_config(void);

#endif
