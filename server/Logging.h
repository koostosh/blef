#pragma once

#include "Timestamp.h"

#include <cstdio>

template <typename... Args>
int log(const char* format, const Args... rest)
{
    char timestamp[80];
    getTimestamp(timestamp, 80);

    printf("%s: ", timestamp);
    printf(format, rest...);
    printf("\n");

    FILE * file;
    file = fopen("server.log", "a");
    fprintf(file, "%s: ", timestamp);
    fprintf(file, format, rest...);
    fprintf(file, "\n");
    fclose(file);

    return 0; // TODO return meaningful codes
}
