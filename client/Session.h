#pragma once
#include "Defines.h"
#include "MainSocket.h"

enum connectionState
{
    CSTATE_DISCONECTED, CSTATE_FIST_TICK, CSTATE_HOSTNAME, CSTATE_USERNAME, CSTATE_PASSWORD, CSTATE_AUTH, CSTATE_CONNECTED,
};

class CAuthHandler;

class Session
{
public:
    Session();
    ~Session() {delete m_inPacket;}

    void InitData(char* host, char* user, char* pass);
    void Update();
    void input(std::string text);
private:
    void processPacket();
    void handleAuth();
    void handleReveal();
    void showHand(uint8 count, uint32 bs);

    // variables
    connectionState m_connectionstate;
    std::string     m_username;
    std::string     m_remoteAddress;

    // objects
    MainSocket      m_sender;
    CAuthHandler*   m_auth;
    IOPacket*       m_inPacket;
};
