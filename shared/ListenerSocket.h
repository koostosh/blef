#pragma once
#include "Defines.h"

class ListenerSocket
{
public:
    ListenerSocket();

    int start_listening(const char* port);
    int get_connection(uint32 waitMS = 0);
    void close_socket();
    bool isListening() { return m_listening; };
private:
    int m_socket;
    bool m_listening;
};
