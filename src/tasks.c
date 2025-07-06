#include "tasks.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

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

int updateTasks(int fileDesc, struct FileHeader *header, struct Task *tasks) {
    if (fileDesc < 0) {
        printf("Invalid file descriptor");
        return STATUS_ERROR;
    }

    header->count = 2;
    header->size = sizeof(struct FileHeader) + (sizeof(struct Task) * header->count);

    if (writeHeader(fileDesc, header) == STATUS_ERROR) return STATUS_ERROR;
    for (int i = 0; i < header->count; i++) {
        if (write(fileDesc, &tasks[i], sizeof(struct Task)) == STATUS_ERROR) {
            perror("write");
            return STATUS_ERROR;
        }
    }

    return STATUS_SUCCESS;
}
