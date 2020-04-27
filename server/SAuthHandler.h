#pragma once
#include "ByteBuffer.h"
#include "SessionMgr.h"

class SAuthHandler
{
public:
    SAuthHandler(SessionMgr* p_manager) : m_manager(p_manager) {};
    uint8 handleAuth(IOPacket* io);
    uint8* getKey() { return K; };
    std::string getUsername() { return ad ? ad->name : ""; };
    uint32 getAccid() { return ad ? ad->Id : 0; };
private:
    void MagicVoid();
    bool MagicBool();
    SessionMgr* m_manager;
    AccountData* ad;
    uint8 s[32], B[32], A[32], M1[20], M2[20], v[32], b[20], K[20];

    const uint8 N[32] = { 0xB7, 0x9B, 0x3E, 0x2A, 0x87, 0x82, 0x3C, 0xAB, 0x8F, 0x5E, 0xBF, 0xBF, 0x8E, 0xB1, 0x01, 0x08,
        0x53, 0x50, 0x06, 0x29, 0x8B, 0x5B, 0xAD, 0xBD, 0x5B, 0x53, 0xE1, 0x89, 0x5E, 0x64, 0x4B, 0x89 };
    const uint8 g = 7;
};
