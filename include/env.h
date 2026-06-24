#include <string.h>
#include <stdlib.h>

#include "charvec.h"

#ifndef ENV_H
#define ENV_H

charvec* load_path(void);

char* get_home(void);

#endif
