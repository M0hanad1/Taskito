#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "header.h"
#include "tasks.h"

int saveData(int fileDesc, struct FileHeader *header, struct Task *tasks) {
    if (fileDesc < 0) {
        printf("Unvalid file descriptor\n");
        return STATUS_ERROR;
    }

    if (lseek(fileDesc, 0, SEEK_SET) == STATUS_ERROR) {
        perror("lseek");
        return STATUS_ERROR;
    }

    if (write(fileDesc, header, sizeof(struct FileHeader)) == STATUS_ERROR) {
        perror("write");
        return STATUS_ERROR;
    }

    if (ftruncate(fileDesc, header->size) == -1) {
        perror("ftruncate");
        return STATUS_ERROR;
    }

    if (!tasks) return STATUS_SUCCESS;
    for (int i = 0; i < header->count; i++) {
        if (write(fileDesc, &tasks[i], sizeof(struct Task)) == STATUS_ERROR) {
            perror("write");
            return STATUS_ERROR;
        }
    }

    return STATUS_SUCCESS;
}
