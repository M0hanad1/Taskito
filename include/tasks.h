#ifndef _INC_TASKS_H
#define _INC_TASKS_H

#include <stdbool.h>

#include "header.h"

struct Task {
    char title[50];
    char description[200];
    bool done;
};

int getTasks(int fileDesc, struct FileHeader *header, struct Task **tasksOut);
int searchTask(int fileDesc, char *search, struct Task **taskOut);
int addTask(struct FileHeader *header, char *taskArgv, struct Task **tasksOut);

#endif
