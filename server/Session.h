#pragma once
#include "Defines.h"

class SAuthHandler;
class MainSocket;
class IOPacket;
class SessionMgr;

class Session
{
public:
    Session(int p_socket, SessionMgr* p_manager);
    ~Session();

    bool update(uint32 diff);
    void sendPacket(IOPacket* out);
    uint32 getSSID() { return m_ssid; };
    uint32 getAccId() { return m_accid; };
    void kick();
private:
    bool handleAuth();
    void processPacket();

    SessionMgr*     m_manager;
    MainSocket*     m_socket;
    SAuthHandler*   m_auth;

    uint32          m_inactiveTimer;
    std::string     m_username;
    uint32          m_accid;
    uint8           m_connectionstate;
    uint32          m_ssid;
    IOPacket*       m_inPacket;

    static uint32 NewSSID();
};
