#pragma once
#include "ByteBuffer.h"

class CAuthHandler
{
public:
    CAuthHandler(std::string user, std::string pass) : m_user(user), m_pass(pass) {};
    void startAuth(IOPacket* out);
    uint8 handleAuth(IOPacket* io); // if return = AUTH_NOT_YET we have packet to send
    uint8* getKey() { return K; };
private:
    void MagicVoid();
    std::string m_user, m_pass;
    uint8 A[32], B[32], K[20], M1[20], M2[20], s[32];

    const uint8 N[32] = { 0xB7, 0x9B, 0x3E, 0x2A, 0x87, 0x82, 0x3C, 0xAB, 0x8F, 0x5E, 0xBF, 0xBF, 0x8E, 0xB1, 0x01, 0x08,
        0x53, 0x50, 0x06, 0x29, 0x8B, 0x5B, 0xAD, 0xBD, 0x5B, 0x53, 0xE1, 0x89, 0x5E, 0x64, 0x4B, 0x89 };
    const uint8 g = 7;
};
