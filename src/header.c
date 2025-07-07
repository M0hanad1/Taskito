#include "header.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "common.h"
#include "tasks.h"

int createHeader(struct FileHeader **headerOut) {
    struct FileHeader *header = calloc(1, sizeof(struct FileHeader));
    if (!header) {
        printf("Failed to allocate header\n");
        return STATUS_ERROR;
    }

    header->magic = MAGIC_VALUE;
    header->version = VERSION;
    header->count = 0;
    header->size = sizeof(struct FileHeader);
    *headerOut = header;

    return STATUS_SUCCESS;
}

int validateHeader(int fileDesc, struct FileHeader **headerOut) {
    if (fileDesc < 0) {
        printf("Unvalid file descriptor\n");
        return STATUS_ERROR;
    }

    struct FileHeader *header = calloc(1, sizeof(struct FileHeader));
    if (!header) {
        printf("Failed to allocate header\n");
        return STATUS_ERROR;
    }

    if (read(fileDesc, header, sizeof(struct FileHeader)) != sizeof(struct FileHeader)) {
        printf("Failed to read the file\n");
        free(header);
        return STATUS_ERROR;
    }

    if (header->magic != MAGIC_VALUE || header->version != VERSION) {
        printf("Invalid file header values\n");
        free(header);
        return STATUS_ERROR;
    }

    struct stat fileStates = {0};
    if (fstat(fileDesc, &fileStates) == STATUS_ERROR) {
        perror("fstat");
        free(header);
        return STATUS_ERROR;
    }

    if (fileStates.st_size != header->size) {
        printf("Unmatched header and file sizes\n");
        free(header);
        return STATUS_ERROR;
    }

    *headerOut = header;
    return STATUS_SUCCESS;
}
