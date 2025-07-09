#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char *lower(char *value) {
    char *lower = calloc(1, strlen(value) + 1);
    int i = 0;
    for (; value[i] != '\0'; i++) lower[i] = tolower(value[i]);
    lower[i] = '\0';
    return lower;
}
