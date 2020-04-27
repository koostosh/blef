#pragma once
#include "BaseSocket.h"
#include "ByteBuffer.h"

class IOPacket;
class MainSocket : public BaseSocket
{
public:
    MainSocket(): index_s(0), index_r(0), last_s(0), last_r(0), partialLength(0), headerLength(0), currentSize(0), m_enryptHeaders(false)
    {
        memset(headerKey, 0, 20);
        partialPacket = NULL;
    }
    MainSocket(int socket) : MainSocket() { m_socket = socket; m_connected = true; }
    ~MainSocket() { close_socket(); }

    void setKey(uint8* K) { memcpy(headerKey, K, 20); m_enryptHeaders = true; };
    bool recv(IOPacket* in);
    void send(IOPacket* out);
private:
    void encrypt(uint8* data, size_t len);
    void decrypt(uint8* data, size_t len);
    uint8 index_s, index_r, last_s, last_r;
    uint8 headerKey[20];
    bool m_enryptHeaders;

    uint8* partialPacket;
    uint16 partialLength, currentSize;
    uint8 header[4];
    uint8 headerLength;
};
