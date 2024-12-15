#include "set.h"
#include <stdlib.h>

void set_init(Set *set) {
    set->size = 0;
}

bool set_add(Set *set, int value) {
    if (set->size >= MAX_SET_SIZE) {
        return false;
    }

    if (set_contains(set, value)) {
        return false;
    }

    int i = set->size - 1;
    while (i >= 0 && set->elements[i] > value) {
        set->elements[i + 1] = set->elements[i];
        i--;
    }
    set->elements[i + 1] = value;
    set->size++;
    return true;
}

bool set_remove(Set *set, int value) {
    for (int i = 0; i < set->size; i++) {
        if (set->elements[i] == value) {
            for (int j = i; j < set->size - 1; j++) {
                set->elements[j] = set->elements[j + 1];
            }
            set->size--;
            return true;
        }
    }
    return false;
}

bool set_contains(const Set *set, int value) {
    int left = 0, right = set->size - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (set->elements[mid] == value) {
            return true;
        } else if (set->elements[mid] < value) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return false;
}

int set_size(const Set *set) {
    return set->size;
}
