/*
 * fileNameFromDesc.cpp
 *
 *  Created on: 09.06.2020
 *      Author: pantec
 */


#include "fileNameFromDesc.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "tns_util/daemonic.h"

#define MAX_STRLEN	128

char *fileNameFromDesc(int fd, char *fileNameBuffer)
{
	if (fileNameBuffer == NULL) {
		return NULL;
	}

    char proclnk[MAX_STRLEN];

    int fno;
    ssize_t r;

    // test.txt created earlier
    sprintf(proclnk, "/proc/self/fd/%d", fd);
    r = readlink(proclnk, fileNameBuffer, MAX_STRLEN);
    if (r < 0) {
        EPRINTF("failed to readlink: %s", strerror(errno) );
        return NULL;
    }
    fileNameBuffer[r] = '\0';

    printf("fp -> fno -> filename: %d -> %s\n", fd, fileNameBuffer);

    return fileNameBuffer;
}

