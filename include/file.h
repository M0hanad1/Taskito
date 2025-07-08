#ifndef _INC_FILE_H
#define _INC_FILE_H

#include "header.h"
#include "tasks.h"

int saveData(int fileDesc, struct FileHeader *header, struct Task *tasks);

#endif
