#include "parse.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "common.h"

int createHeader(int fileDesc, struct FileHeader **headerOut) {
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

    return writeHeader(fileDesc, header);
}

int writeHeader(int fileDesc, struct FileHeader *header) {
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
        return STATUS_ERROR;
    }
    if (header->magic != MAGIC_VALUE || header->version != VERSION) {
        printf("Invalid file header values\n");
        return STATUS_ERROR;
    }

    struct stat fileStates = {0};
    if (fstat(fileDesc, &fileStates) == STATUS_ERROR) {
        perror("fstat");
        return STATUS_ERROR;
    }
    if (fileStates.st_size != header->size) {
        printf("Unmatched header and file sizes\n");
        return STATUS_ERROR;
    }

    *headerOut = header;
    return STATUS_SUCCESS;
}
