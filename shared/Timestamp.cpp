#include "Timestamp.h"

#include <ctime>

void getTimestamp(char* result, size_t maxsize)
{
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(result, maxsize, "%Y/%m/%d %H:%M:%S", timeinfo);
}
