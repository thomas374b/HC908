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

/**
 * @return pointer to fileName string or NULL in case of error
 * @param fd     unix file descriptor
 * @param fileNameBuffer    location of caller-allocated memory
 *
 * The function works only on Linux since it uses special kernel 
 * features from a mounted /proc filesystem
 */
char *fileNameFromDesc(int fd, char *fileNameBuffer)
{
	if (fileNameBuffer == NULL) {
		return NULL;
	}

    char proclnk[MAX_STRLEN];

    ssize_t r;

    // test.txt created earlier
    sprintf(proclnk, "/proc/self/fd/%d", fd);
    r = readlink(proclnk, fileNameBuffer, MAX_STRLEN);
    if (r < 0) {
        EPRINTF("failed to readlink: %s", strerror(errno) );
        return NULL;
    }
    fileNameBuffer[r] = '\0';

    printf("fd -> filename: %d -> %s\n", fd, fileNameBuffer);

    return fileNameBuffer;
}

