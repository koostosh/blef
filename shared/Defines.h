#pragma once
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef signed int int32;
typedef signed short int16;
typedef signed char int8;

#define DEFAULT_PORT "9412"
#define MAGIC_GAMEID 78652

#define MAX_INACTIVITY 30000
#define PING_DELAY 5000

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#define NO_CARD 100
enum AuthErrors
{
    AUTH_OK                 = 0, // authorized
    AUTH_BAD_DATA           = 1, // unauthorized
    AUTH_ACCOUNT_BANNED     = 2,
    AUTH_ACCOUNT_SUSPENDED  = 3,
    AUTH_BROKEN_DATA        = 4, // malformed packet, send to client as BAD_DATA
    AUTH_NOT_YET            = 5, // authorization not finished
    AUTH_OUTDATED_CLIENT    = 6, // HUE
    AUTH_NO_ACCOUNT         = 7, // no such account, send to client as BAD_DATA
};
