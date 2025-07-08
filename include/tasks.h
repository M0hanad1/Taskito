#ifndef _INC_TASKS_H
#define _INC_TASKS_H

#include <stdbool.h>

#include "header.h"

struct Task {
    char title[51];
    char description[201];
    bool done;
};

int searchTasks(struct FileHeader *header, struct Task *tasks, char *search, struct Task **tasksOut);
int getTasks(int fileDesc, struct FileHeader *header, struct Task **tasksOut);
int addTask(struct FileHeader *header, char *taskArgv, struct Task **tasksOut);
int removeTask(struct FileHeader *header, char *search, struct Task **tasksOut);
int doneTask(struct FileHeader *header, char *search, struct Task *tasksOut);
void printTasks(struct Task *tasks, int count);

#endif
