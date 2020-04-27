#pragma once
#include "Defines.h"

class BaseSocket
{
public:
    BaseSocket();
    bool isConnected() { return m_connected; }
    bool open_socket(const char* address, const char* port);
    void close_socket();
protected:
    void send_raw(uint8* buffer, uint16 datalength);
    bool recv_raw(uint8* buffer, uint16* datalength);

    bool m_connected;
    int m_socket;
};

