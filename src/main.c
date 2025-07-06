#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "common.h"
#include "parse.h"

typedef struct {
    char *file;
    bool new;
    bool delete;
    bool append;
} Flags;

typedef struct {
    char *title;
    char *description;
    bool done;
} Task;

void usage(char *argv[]) {
    printf("Usage: %s -n -f <database_name>\n", argv[0]);
    printf("\t -f - (required) Path to database file\n");
    printf("\t -a - Append to the database\n");
    printf("\t -n - Create a new database file\n");
    printf("\t -d - Delete the whole database\n");
}

int main(int argc, char *argv[]) {
    Flags flags = {0};
    char c = 0;

    while ((c = getopt(argc, argv, "ndf:")) != STATUS_ERROR) {
        switch (c) {
            case 'n':
                flags.new = true;
                break;
            case 'a':
                flags.append = true;
                break;
            case 'd':
                flags.delete = true;
                break;
            case 'f':
                flags.file = optarg;
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

    int fileDesc = open(flags.file, O_RDWR | (flags.new << 8 & O_CREAT));

    if (fileDesc == STATUS_ERROR) {
        if (!flags.new) {
            printf("Database file doesn't exist\n");
        } else {
            printf("Couldn't create the database file");
        }
        return 0;
    }

    printf("Create new file: %d\n", flags.new);
    printf("File path: %s\n", flags.file);
    printf("File descriptor: %d\n", fileDesc);

    struct FileHeader *header = &(struct FileHeader){0};
    int parseOutput = 0;

    if (flags.new) {
        parseOutput = createHeader(fileDesc, &header);
    } else {
        parseOutput = validateHeader(fileDesc, &header);
    }

    if (parseOutput == STATUS_ERROR) return 0;

    printf("Header magic: %u\n", header->magic);
    printf("Header version: %hu\n", header->version);
    printf("Header count: %hu\n", header->count);
    printf("Header size: %ld\n", header->size);
}
