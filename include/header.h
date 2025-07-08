#ifndef _INC_HEADER_H
#define _INC_HEADER_H

#define MAGIC_VALUE 0x4D4F48
#define VERSION 1.0

struct FileHeader {
    unsigned magic;
    double version;
    unsigned short count;
    long size;
};

int createHeader(struct FileHeader **header);
int validateHeader(int fileDesc, struct FileHeader **header);

#endif
