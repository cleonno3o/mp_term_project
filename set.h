#ifndef SET_H
#define SET_H

#include <stdbool.h>

#define MAX_SET_SIZE 100

typedef struct {
    int elements[MAX_SET_SIZE];
    int size;
} Set;

void set_init(Set *set);
bool set_add(Set *set, int value);
bool set_remove(Set *set, int value);
bool set_contains(const Set *set, int value);
int set_size(const Set *set);

#endif // SET_H
