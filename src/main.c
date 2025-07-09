#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "header.h"
#include "tasks.h"

#define HANDLE_SAVE saveData(fileDesc, header, tasks) == STATUS_ERROR

#define HANDLE_TASK(flag, taskFunc, tasksIn, msg)                                 \
    if (flags.flag) {                                                             \
        entered = true;                                                           \
        if (taskFunc(header, flags.flag, tasksIn) == STATUS_ERROR || HANDLE_SAVE) \
            goto cleanup;                                                         \
        printf(msg "\n");                                                         \
    }

struct Flags {
    char *file;
    char *add;
    char *search;
    char *rem;
    char *mark;
    bool new;
    bool get;
    bool delete;
};

void usage(char *argv[]) {
    printf("Usage: %s -n -f <database_name> -a <title,description,done>\n", argv[0]);
    printf("\t -f - (required) Path to database file: \"-f <database_name>\"\n");
    printf("\t -a - Add new task to the database: \"-a <title,description,done>\"\n");
    printf("\t -s - Search for tasks in the database: \"-s <search_value>\"\n");
    printf("\t -r - Remove a task from the database: \"-r <search_value>\"\n");
    printf("\t -m - Mark task to the opposite: \"-m <search_value>\"\n");
    printf("\t -g - get all tasks\n");
    printf("\t -n - Create a new database file\n");
    printf("\t -d - Delete the whole database\n");
}

int main(int argc, char *argv[]) {
    struct Task *tasks = NULL;
    struct FileHeader *header = NULL;
    int fileDesc = 0;
    struct Flags flags = {0};
    char c = 0;
    bool entered = false;

    while ((c = getopt(argc, argv, "ndga:m:s:r:f:")) != STATUS_ERROR) {
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
                flags.rem = optarg;
                break;
            case 'm':
                flags.mark = optarg;
                break;
            default:
                usage(argv);
                goto cleanup;
        }
    }

    if (!flags.file) {
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

    if (flags.new) {
        if ((fileDesc = open(flags.file, O_RDWR | O_CREAT, 0666)) == STATUS_ERROR) {
            printf("Couldn't create the database file\n");
            goto cleanup;
        }
        if (createHeader(&header) == STATUS_ERROR || HANDLE_SAVE) goto cleanup;
        printf("Database file is created successfully\n");
    } else {
        if ((fileDesc = open(flags.file, O_RDWR, 0666)) == STATUS_ERROR) {
            printf("Couldn't read the database file\n");
            goto cleanup;
        }
        if (validateHeader(fileDesc, &header) == STATUS_ERROR) goto cleanup;
    }

    if (getTasks(fileDesc, header, &tasks) == STATUS_ERROR) goto cleanup;

    HANDLE_TASK(add, addTask, &tasks, "Tasks are added successfully");
    HANDLE_TASK(mark, doneTask, tasks, "Task mark is changed successfully");
    HANDLE_TASK(rem, removeTask, &tasks, "Task is removed successfully");
    if (entered || flags.get) printTasks(tasks, header->count);

    if (flags.search) {
        struct Task *tasksSearch = NULL;
        int count = searchTasks(header, tasks, flags.search, &tasksSearch);
        if (count > 0) printf("Tasks are found\n");
        printTasks(tasksSearch, count);
        free(tasksSearch);
    }

    goto cleanup;

cleanup:
    free(tasks);
    free(header);
    close(fileDesc);
    return 0;
}
