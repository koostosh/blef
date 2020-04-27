#include "ListenerSocket.h"
#include "Util.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

ListenerSocket::ListenerSocket()
{
    m_socket = INVALID_SOCKET;
    m_listening = false;
}

int ListenerSocket::start_listening(const char* port)
{
    int IsError;
    sockaddr_in service;

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
        return SOCKET_ERROR;

    service.sin_family = AF_INET;
    service.sin_port = htons(atoi(port));
    service.sin_addr.s_addr = INADDR_ANY;
    int val =1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));
    IsError = bind(m_socket, (struct sockaddr*)&service, sizeof(service));
    if (IsError == SOCKET_ERROR)
    {
        close(m_socket);
        return SOCKET_ERROR;
    }

    IsError = listen(m_socket, SOMAXCONN);
    if (IsError == SOCKET_ERROR)
    {
        close(m_socket);
        return SOCKET_ERROR;
    }

    m_listening = true;
    return 0;
}

int ListenerSocket::get_connection(uint32 waitMS)
{
    if (!m_listening)
        return INVALID_SOCKET;
    fd_set recvset;
    timeval tv;

    FD_ZERO(&recvset);
    FD_SET(m_socket, &recvset);
    tv.tv_sec = waitMS / 1000;
    tv.tv_usec = waitMS % 1000;
    if (select(m_socket + 1, &recvset, NULL, NULL, &tv) == 1)
    {
        return accept(m_socket, NULL, NULL);
    }

    return INVALID_SOCKET;
}

void ListenerSocket::close_socket()
{
    close(m_socket);
    m_socket = INVALID_SOCKET;
    m_listening = false;
}
