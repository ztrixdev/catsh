
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef charvec_H

#define charvec_H

#define BASIC_CAPACITY 8
#define CAPACITY_MULTIPLIER 2

typedef struct charvec charvec; // dynamic array of strings

// creates a new dynamic array
charvec* charvec_init(void);
// appends a new value to target
void charvec_append(charvec* target, char* value);
// returns the index of value if found in target, if not returns -1
int64_t charvec_index_of(charvec* target, char* value);
// returns the value stored on idx
char* charvec_get(charvec* target, size_t idx);
// get null-terminated data pointer
char** charvec_get_nterm_dataptr(charvec* target);
// removes value from target
void charvec_remove(charvec* target, char* value);
// returns the length (total amount of elements inside) of a dynamic array
size_t charvec_length(charvec* target);

#endif // charvec_H
