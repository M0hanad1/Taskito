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
int updateTasks(int fileDesc, struct FileHeader *header, struct Task *tasks);

#endif
