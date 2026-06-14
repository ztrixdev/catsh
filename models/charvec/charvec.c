#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "charvec.h"

typedef struct charvec
{
    char** data;
    size_t len;
    size_t cap;
} charvec;

// creates a new dynamic array
charvec* charvec_init(void)
{
    charvec *d = malloc(sizeof(charvec));
    d->cap = 0;
    d->len = 0;
    d->data = NULL;
    return d;
}

// appends a new value to target
void charvec_append(charvec* target, char* value)
{
    if (target->len == 0 && target->data == NULL)
    {
        target->data = malloc(BASIC_CAPACITY * sizeof(char*));
        target->data[0] = value;
        target->cap = BASIC_CAPACITY;
        target->len = 1;
    }
    else
    {
        if (target->cap - target->len <= 2)
        {
            // recreate the array with CAPACITY_MULTIPLIER times the capacity
            char **new = malloc(target->cap * CAPACITY_MULTIPLIER * sizeof(char*));
            for (size_t idx = 0; idx < target->len; idx++)
                new[idx] = target->data[idx];
            free(target->data);
            target->data = new;
            target->cap = target->cap * CAPACITY_MULTIPLIER;
        }

        target->data[target->len] = value;
        target->len++;
    }
}

// returns the index of value if found in target, if not returns -1
int64_t charvec_index_of(charvec* target, char* value)
{
    // O(N) lookup
    for (size_t idx = 0; idx < target->len; idx++)
    {
        if (strcmp(target->data[idx], value)==0)
            return idx;
    }

    return -1;
}

char* charvec_get(charvec* target, size_t idx)
{
    if (idx > target->len)
    {
        puts("Index out of range");
    }

    return target->data[idx];
}

void charvec_remove(charvec* target, char* value)
{
//todo
}

char** charvec_get_nterm_dataptr(charvec* target)
{
    target->data[target->cap] = NULL;
    return target->data;
}

size_t charvec_length(charvec* target)
{
    return target->len;
}
