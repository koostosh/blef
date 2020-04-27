#include "BaseSocket.h"
#include "Util.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>

BaseSocket::BaseSocket()
{
    m_socket = INVALID_SOCKET;
    m_connected = false;
}

bool BaseSocket::open_socket(const char* address, const char* port)
{
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int IsError;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    IsError = getaddrinfo(address, port, &hints, &result);
    if (IsError != 0)
    {
        return false;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        m_socket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);

        if (m_socket == INVALID_SOCKET)
        {
            return false;
        }

        IsError = connect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (IsError == SOCKET_ERROR)
        {
            close_socket();
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (m_socket == INVALID_SOCKET)
    {
        return false;
    }

    uint32 ms = 100;
    setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)(&ms), sizeof(uint32));

    m_connected = true;
    return true;
}

void BaseSocket::send_raw(uint8* buffer, uint16 datalength)
{
    int IsError = send(m_socket, (char*)buffer, datalength, 0);
    if (IsError == SOCKET_ERROR)
    {
        close_socket();
    }
}

bool BaseSocket::recv_raw(uint8* buffer, uint16* datalength)
{
    fd_set recvset;
    timeval tv;
    int IsError = 0;

    FD_ZERO(&recvset);
    FD_SET(m_socket, &recvset);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    if (select(m_socket + 1, &recvset, NULL, NULL, &tv) == 1)
    {
        IsError = recv(m_socket, (char*)buffer, *datalength, 0);
        if (IsError <= 0)
        {
            *datalength = -IsError;
            return false;
        }
        *datalength = (uint16)IsError;
        return true;
    }

    *datalength = 0;
    return true;
}

void BaseSocket::close_socket()
{
    close(m_socket);
    m_socket = INVALID_SOCKET;
    m_connected = false;
}

