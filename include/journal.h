#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "charvec.h"

#ifndef JOURNAL_H
#define JOURNAL_H


#define JOURNAL_FILENAME ".catsh_journal"
#define MAX_JOURNAL_ENTRY_LEN 2048

char* get_journal_path(void);
bool journal_file_exists(void);
FILE* get_journal_file_ptr(char *mode);
void init_journal_file(void);
void clear_journal(charvec* jcv);
charvec* read_from_journal(void);
void append_to_journal(charvec* jcv, char* entry);

#endif
