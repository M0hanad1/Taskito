#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "header.h"
#include "parse.h"
#include "tasks.h"

struct Flags {
    char *file;
    char *add;
    char *search;
    char *remove;
    bool new;
    bool get;
    bool delete;
};

void usage(char *argv[]) {
    printf("Usage: %s -n -f <database_name>\n", argv[0]);
    printf("\t -f - (required) Path to database file\n");
    printf("\t -r - get all tasks\n");
    printf("\t -n - Create a new database file\n");
    printf("\t -d - Delete the whole database\n");
}

int main(int argc, char *argv[]) {
    struct Task *tasks = calloc(1, sizeof(struct Task));
    struct FileHeader *header = calloc(1, sizeof(struct FileHeader));
    int parseOutput = 0;
    struct Flags flags = {0};
    char c = 0;

    while ((c = getopt(argc, argv, "na:dgs:r:f:")) != STATUS_ERROR) {
        switch (c) {
            case 'n':
                flags.new = true;
                break;
            case 'g':
                flags.get = true;
                break;
            case 'd':
                flags.delete = true;
                break;
            case 'f':
                flags.file = optarg;
                break;
            case 'a':
                flags.add = optarg;
                break;
            case 's':
                flags.search = optarg;
                break;
            case 'r':
                flags.remove = optarg;
                break;
            case '?':
                printf("Unkown option: %c\n", c);
                break;
            default:
                return STATUS_ERROR;
        }
    }

    if (flags.file == NULL) {
        printf("File path is required\n");
        usage(argv);
        goto cleanup;
    }

    if (flags.delete) {
        if (remove(flags.file) == STATUS_ERROR) {
            printf("Couldn't delete the database\n");
        } else {
            printf("Database deleted successfully\n");
        }
        goto cleanup;
    }

    int fileDesc = open(flags.file, O_RDWR | (flags.new << 8 & O_CREAT), 0666);

    if (fileDesc == STATUS_ERROR) {
        if (!flags.new) {
            printf("Database file doesn't exist\n");
        } else {
            printf("Couldn't create the database file");
        }
        goto cleanup;
    }

    printf("Create new file: %d\n", flags.new);
    printf("File path: %s\n", flags.file);
    printf("File descriptor: %d\n", fileDesc);

    if (flags.new) {
        parseOutput = createHeader(&header);
        if (saveData(fileDesc, header, NULL) == STATUS_ERROR) goto cleanup;
    } else {
        parseOutput = validateHeader(fileDesc, &header);
    }

    if (parseOutput == STATUS_ERROR) goto cleanup;

    printf("Header magic: %u\n", header->magic);
    printf("Header version: %hu\n", header->version);
    printf("Header count: %hu\n", header->count);
    printf("Header size: %ld\n", header->size);

    if (getTasks(fileDesc, header, &tasks) == STATUS_ERROR) goto cleanup;

    if (flags.add) {
        if (addTask(header, flags.add, &tasks) == STATUS_ERROR || saveData(fileDesc, header, tasks) == STATUS_ERROR) goto cleanup;
        printf("Tasks are added successfully\n");
    }

    if (flags.get) {
        if (header->count == 0) {
            printf("There's no tasks to read\n");
        } else {
            for (int i = 0; i < header->count; i++) {
                printf("=====\nTitle: %s\nDescription: %s\nDone: %d\n=====\n", tasks[i].title, tasks[i].description, tasks[i].done);
            }
        }
    }

    if (flags.search) {
        struct Task *tasksSearch = {0};
        int count = 0;
        if ((count = searchTask(header, tasks, flags.search, &tasksSearch, false)) < 1) {
            free(tasksSearch);
            goto cleanup;
        }
        for (int i = 0; i < count; i++) {
            printf("=====\nTitle: %s\nDescription: %s\nDone: %d\n=====\n", tasksSearch[i].title, tasksSearch[i].description, tasksSearch[i].done);
        }
    }

    if (flags.remove) {
        if (removeTask(header, flags.remove, &tasks) == STATUS_ERROR || saveData(fileDesc, header, tasks) == STATUS_ERROR) goto cleanup;
        for (int i = 0; i < header->count; i++) {
            printf("=====\nTitle: %s\nDescription: %s\nDone: %d\n=====\n", tasks[i].title, tasks[i].description, tasks[i].done);
        }
    }

    printf("Header count: %hu\n", header->count);
    printf("Header size: %ld\n", header->size);
    goto cleanup;

cleanup:
    free(tasks);
    free(header);
    close(fileDesc);
    return 0;
}
