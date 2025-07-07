#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *lower(char *value) {
    char *lower = calloc(1, strlen(value));
    for (int i = 0; value[i] != '\0'; i++) lower[i] = tolower(value[i]);
    return lower;
}
