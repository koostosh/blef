#include "Session.h"
#include "CAuthHandler.h"
#include "Util.h"
#include <algorithm>

Session::Session() : m_connectionstate(CSTATE_FIST_TICK)
{
    m_inPacket = new IOPacket;
}

void Session::InitData(char* p_host, char* p_user, char* p_pass)
{
    m_remoteAddress = p_host;
    m_username = p_user;
    if (m_sender.open_socket(p_host, DEFAULT_PORT))
    {
        printf("connected!\n");
    }
    else
    {
        printf( "cannot connect\n");
        m_connectionstate = CSTATE_DISCONECTED;
    }
    IOPacket l_auth;
    m_auth = new CAuthHandler(m_username, p_pass);
    m_auth->startAuth(&l_auth);
    m_sender.send(&l_auth);
    m_connectionstate = CSTATE_AUTH;
}

void Session::Update()
{
    switch (m_connectionstate)
    {
    case CSTATE_DISCONECTED: // not connected
        break;
    case CSTATE_FIST_TICK: // first tick
        printf("Enter host address\n");
        m_connectionstate = CSTATE_HOSTNAME;
        break;
    case CSTATE_AUTH: // during auth
    {
        if (m_sender.recv(m_inPacket))
        {
            if (m_inPacket->getOpcode() != SMSG_AUTH)
                return;
            handleAuth();
        }
        break;
    }
    case CSTATE_CONNECTED: // post auth
    {
        while (m_sender.recv(m_inPacket))
        {
            processPacket();
        }

        if (!m_sender.isConnected())
        {
            m_connectionstate = CSTATE_DISCONECTED;
            printf("Connection lost\n");
        }
        break;
    }
    }
}

void Session::input(std::string text)
{
    switch (m_connectionstate)
    {
    case CSTATE_HOSTNAME: // host
        m_remoteAddress = text;
        printf("Enter username\n");
        m_connectionstate = CSTATE_USERNAME;
        break;
    case CSTATE_USERNAME: // user
        printf("Enter password\n");
        m_username = text;
        m_connectionstate = CSTATE_PASSWORD;
        break;
    case CSTATE_PASSWORD: // pass
    {
        if (m_sender.open_socket(m_remoteAddress.c_str(), DEFAULT_PORT))
        {
            printf("connected!\n");
        }
        else
        {
            printf("cannot connect\n");
            printf("%s\n", hashFrom(m_username, text).c_str());
            m_connectionstate = CSTATE_DISCONECTED;
            break;
        }
        IOPacket io;
        m_auth = new CAuthHandler(m_username, text);
        m_auth->startAuth(&io);
        m_sender.send(&io);
        m_connectionstate = CSTATE_AUTH;
        break;
    }
    case CSTATE_CONNECTED: // chat
    {
        IOPacket io;
        io.reset(text.size() + 1, MSG_TEXT_MESSAGE);
        io << text;
        m_sender.send(&io);
        break;
    }
    default:
        break;
    }
}

void Session::processPacket()
{
    switch (m_inPacket->getOpcode())
    {
    case MSG_TEXT_MESSAGE:
    {
        std::string who, what;
        *m_inPacket >> who >> what;
        std::replace(what.begin(), what.end(), '\n', ' ');
        std::replace(who.begin(), who.end(), '\n', ' ');
        printf("%s: %s\n", who.c_str(), what.c_str());
        break;
    }
    case SMSG_USER_JOINED:
    {
        std::string who;
        *m_inPacket >> who;
        printf("Player %s joined.\n", who.c_str());
        break;
    }
    case SMSG_USER_LEFT:
    {
        std::string who;
        *m_inPacket >> who;
        printf("Player %s left.\n", who.c_str());
        break;
    }
    case MSG_PING_PONG:
    {
        m_sender.send(m_inPacket);
        break;
    }
    case SMSG_BOARD_STATE:
    {
        std::string state;
        *m_inPacket >> state;
        printf(">%s<\n", state.c_str());
        break;
    }
    }
}

void Session::handleAuth()
{
    uint8 result = m_auth->handleAuth(m_inPacket);
    switch (result)
    {
    case AUTH_OK:
        m_sender.setKey(m_auth->getKey()); // from now communication is encrypted
        m_connectionstate = CSTATE_CONNECTED;
        memset(m_auth, 0x00, sizeof(CAuthHandler));
        delete m_auth;
        printf("authorization successful\n");
        break;
    case AUTH_BAD_DATA:
        printf("autorizing failed, invalid username or password\n");
        m_connectionstate = CSTATE_DISCONECTED;
        break;
    case AUTH_NOT_YET:
        m_sender.send(m_inPacket);
        break;
    }
}
