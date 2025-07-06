#ifndef _INC_PARSER_HEADER
#define _INC_PARSER_HEADER

#define MAGIC_VALUE 0x4D4F48
#define VERSION 1.0

struct FileHeader {
    unsigned magic;
    unsigned short version;
    unsigned short count;
    long size;
};

int createHeader(int fileDesc, struct FileHeader **header);
int validateHeader(int fileDesc, struct FileHeader **header);
int writeHeader(int fileDesc, struct FileHeader *header);

#endif
