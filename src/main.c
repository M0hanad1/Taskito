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
    bool new;
    bool read;
    bool delete;
};

void usage(char *argv[]) {
    printf("Usage: %s -n -f <database_name>\n", argv[0]);
    printf("\t -f - (required) Path to database file\n");
    printf("\t -r - Read all tasks\n");
    printf("\t -n - Create a new database file\n");
    printf("\t -d - Delete the whole database\n");
}

int main(int argc, char *argv[]) {
    struct Flags flags = {0};
    char c = 0;

    while ((c = getopt(argc, argv, "na:ds:rf:")) != STATUS_ERROR) {
        switch (c) {
            case 'n':
                flags.new = true;
                break;
            case 'r':
                flags.read = true;
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
        return 0;
    }

    if (flags.delete) {
        if (remove(flags.file) == STATUS_ERROR) {
            printf("Couldn't delete the database\n");
        } else {
            printf("Database deleted successfully\n");
        }
        return 0;
    }

    int fileDesc = open(flags.file, O_RDWR | (flags.new << 8 & O_CREAT), 0666);

    if (fileDesc == STATUS_ERROR) {
        if (!flags.new) {
            printf("Database file doesn't exist\n");
        } else {
            printf("Couldn't create the database file");
        }
        close(fileDesc);
        return 0;
    }

    printf("Create new file: %d\n", flags.new);
    printf("File path: %s\n", flags.file);
    printf("File descriptor: %d\n", fileDesc);

    struct FileHeader *header = &(struct FileHeader){0};
    int parseOutput = 0;

    if (flags.new) {
        parseOutput = createHeader(&header);
        saveData(fileDesc, header, NULL);
    } else {
        parseOutput = validateHeader(fileDesc, &header);
    }

    if (parseOutput == STATUS_ERROR) {
        close(fileDesc);
        return 0;
    };

    printf("Header magic: %u\n", header->magic);
    printf("Header version: %hu\n", header->version);
    printf("Header count: %hu\n", header->count);
    printf("Header size: %ld\n", header->size);

    struct Task *tasks = {0};

    if (getTasks(fileDesc, header, &tasks) == STATUS_ERROR) {
        close(fileDesc);
        free(tasks);
        return 0;
    }

    if (flags.add) {
        if (addTask(header, flags.add, &tasks) == STATUS_ERROR || saveData(fileDesc, header, tasks) == STATUS_ERROR) {
            close(fileDesc);
            return 0;
        };
        printf("Tasks are added successfully\n");
    }

    if (flags.read) {
        for (int i = 0; i < header->count; i++) {
            printf("=====\nTitle: %s\nDescription: %s\nDone: %d\n=====\n", tasks[i].title, tasks[i].description, tasks[i].done);
        }
    }

    if (flags.search) {
        struct Task *taskSearch = {0};
        int count = 0;
        if ((count = searchTask(fileDesc, flags.search, &taskSearch)) == STATUS_ERROR) {
            printf("Task not found\n");
        } else {
            for (int i = 0; i < count; i++) {
                printf("=====\nTitle: %s\nDescription: %s\nDone: %d\n=====\n", taskSearch[i].title, taskSearch[i].description, taskSearch[i].done);
            }
        }
    }

    free(tasks);
    close(fileDesc);
    printf("Header count: %hu\n", header->count);
    printf("Header size: %ld\n", header->size);
}
