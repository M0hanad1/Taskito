#include "tasks.h"

#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "header.h"
#include "utils.h"

static bool checkMatching(struct Task *task, char *value) {
    char *title = lower(task->title);
    char *description = lower(task->description);
    char *search = lower(value);
    bool result = false;

    if (strstr(title, search) || strstr(description, search)) result = true;
    free(title);
    free(description);
    free(search);
    return result;
}

int getTasks(int fileDesc, struct FileHeader *header, struct Task **tasksOut) {
    if (fileDesc < 0) {
        printf("Invalid file descriptor");
        return STATUS_ERROR;
    }

    if (header->count == 0) {
        *tasksOut = NULL;
        return STATUS_SUCCESS;
    }

    struct Task *tasks = calloc(header->count, sizeof(struct Task));
    if (!tasks) {
        printf("Failed to allocate memory for tasks\n");
        return STATUS_ERROR;
    }

    if (lseek(fileDesc, sizeof(struct FileHeader), SEEK_SET) == STATUS_ERROR) {
        perror("lseek");
        return STATUS_ERROR;
    }

    if (read(fileDesc, tasks, header->size - sizeof(struct FileHeader)) == STATUS_ERROR) {
        printf("Failed to read tasks from file\n");
        free(tasks);
        return STATUS_ERROR;
    }

    *tasksOut = tasks;
    return STATUS_SUCCESS;
}

int searchTasks(struct FileHeader *header, struct Task *tasks, char *search, struct Task **tasksOut) {
    int count = 0;

    for (int i = 0; i < header->count; i++) {
        if (!checkMatching(&tasks[i], search)) continue;
        count++;

        struct Task *temp = realloc(*tasksOut, count * sizeof(struct Task));
        if (!temp) {
            printf("Failed to reallocate memory for searching\n");
            return STATUS_ERROR;
        }

        temp[count - 1] = tasks[i];
        *tasksOut = temp;
    }

    return count;
}

int addTask(struct FileHeader *header, char *taskArgv, struct Task **tasksOut) {
    struct Task newTask = {0};
    int titleSize = sizeof(newTask.title) - 1;
    int descriptionSize = sizeof(newTask.description) - 1;
    char *title = strtok(taskArgv, ",");
    char *description = strtok(NULL, ",");
    char *done = strtok(NULL, ",");

    if (!(title && description && done)) {
        printf("You must provide three values to create a task (separated by ',')\n");
        return STATUS_ERROR;
    }

    if ((int)strlen(title) > titleSize) {
        printf("Task title max length is only '%d' characters long\n", titleSize);
        return STATUS_ERROR;
    }

    if ((int)strlen(description) > descriptionSize) {
        printf("Task description max length is only '%d' characters long\n", descriptionSize);
        return STATUS_ERROR;
    }

    strncpy(newTask.title, title, titleSize);
    strncpy(newTask.description, description, descriptionSize);
    newTask.done = done[0] == '1' ? 1 : 0;

    struct Task *temp = realloc(*tasksOut, (header->count + 1) * sizeof(struct Task));

    if (!temp) {
        printf("Failed to reallocate memory\n");
        return STATUS_ERROR;
    }

    header->count++;
    header->size = sizeof(struct FileHeader) + (sizeof(struct Task) * header->count);

    temp[header->count - 1] = newTask;
    *tasksOut = temp;
    return STATUS_SUCCESS;
}

int removeTask(struct FileHeader *header, char *search, struct Task **tasksOut) {
    bool removed = false;

    for (int i = 0; i < header->count; i++) {
        if (!removed && checkMatching(&(*tasksOut)[i], search)) removed = true;
        if (removed && i < header->count - 1) (*tasksOut)[i] = (*tasksOut)[i + 1];
    }

    if (!removed) {
        printf("Task isn't found\n");
        return STATUS_ERROR;
    }

    header->count--;
    header->size -= sizeof(struct Task);

    if (header->count == 0) {
        free(*tasksOut);
        *tasksOut = NULL;
        return STATUS_SUCCESS;
    }

    struct Task *temp = realloc(*tasksOut, sizeof(struct Task) * header->count);
    if (!temp) {
        printf("Failed to reallocate memory\n");
        return STATUS_ERROR;
    }

    *tasksOut = temp;
    return STATUS_SUCCESS;
}

int doneTask(struct FileHeader *header, char *search, struct Task *tasksOut) {
    for (int i = 0; i < header->count; i++) {
        if (checkMatching(&tasksOut[i], search)) {
            tasksOut[i].done = !(tasksOut[i].done);
            return STATUS_SUCCESS;
        }
    }

    printf("Task isn't found\n");
    return STATUS_ERROR;
}

void printTasks(struct Task *tasks, int count) {
    printf("============\n");
    for (int i = 0; i < count; i++) {
        printf("Title: %s\nDescription: %s\nDone: %s\n============\n", tasks[i].title, tasks[i].description, tasks[i].done ? "Yes" : "No");
    }
    if (count < 1) printf("There's no tasks\n============\n");
}
