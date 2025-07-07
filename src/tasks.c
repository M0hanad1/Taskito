#include "tasks.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "header.h"

int getTasks(int fileDesc, struct FileHeader *header, struct Task **tasksOut) {
    if (fileDesc < 0) {
        printf("Invalid file descriptor");
        return STATUS_ERROR;
    }

    struct Task *tasks = calloc(header->count, sizeof(struct Task));
    if (!tasks) {
        printf("Failed to allocate memory for tasks\n");
        return STATUS_ERROR;
    }

    if (read(fileDesc, tasks, header->size) != (int)(header->size - sizeof(struct FileHeader))) {
        printf("Failed to read tasks from file\n");
        free(tasks);
        return STATUS_ERROR;
    }

    *tasksOut = tasks;
    return STATUS_SUCCESS;
}

int searchTask(int fileDesc, char *search, struct Task **taskOut) {
    struct Task currTask = {0};
    int count = 0;
    int readValue = 0;

    if (lseek(fileDesc, sizeof(struct FileHeader), SEEK_SET) == STATUS_ERROR) {
        perror("lseek");
        return STATUS_ERROR;
    }

    while ((readValue = read(fileDesc, &currTask, sizeof(struct Task))) > 0) {
        if (!strstr(currTask.title, search) && !strstr(currTask.description, search)) continue;
        count++;
        *taskOut = realloc(*taskOut, count * sizeof(struct Task));
        if (!taskOut) {
            printf("Failed to reallocate memory for searching\n");
            free(*taskOut);
            return STATUS_ERROR;
        }
        (*taskOut)[count - 1] = currTask;
    }

    if (readValue == STATUS_ERROR) {
        perror("read");
        free(*taskOut);
        return STATUS_ERROR;
    }

    if (count == 0) {
        printf("Task isn't found\n");
        free(*taskOut);
        return STATUS_ERROR;
    }

    return count;
}

int addTask(struct FileHeader *header, char *taskArgv, struct Task **tasksOut) {
    if (strlen(taskArgv) < 3) {
        printf("You must provide three values to create a task (separated by ', ')\n");
        return STATUS_ERROR;
    }

    struct Task newTask = {0};

    strcpy(newTask.title, strtok(taskArgv, ","));
    strcpy(newTask.description, strtok(NULL, ","));
    newTask.done = strtok(NULL, ",")[0] == '1' ? 1 : 0;

    *tasksOut = realloc(*tasksOut, (header->count + 1) * sizeof(struct Task));

    if (!*tasksOut) {
        printf("Failed to reallocate memory\n");
        free(*tasksOut);
        return STATUS_ERROR;
    }

    header->count++;
    header->size = sizeof(struct FileHeader) + (sizeof(struct Task) * header->count);

    (*tasksOut)[header->count - 1] = newTask;
    return STATUS_SUCCESS;
}
