#include "Session.h"
#include "ByteBuffer.h"
#include "SessionMgr.h"
#include "SAuthHandler.h"
#include "MainSocket.h"
#include "Util.h"
#include "Board.h"
#include "Logging.h"

Session::Session(int p_socket, SessionMgr* p_manager) : m_inactiveTimer(0), m_manager(p_manager)
{
    m_ssid = NewSSID();
    m_auth = new SAuthHandler(p_manager);
    m_socket = new MainSocket(p_socket);
    log("ssid%u: creating \n", m_ssid);
    m_connectionstate = 1;
    m_inPacket = new IOPacket;
}

Session::~Session()
{
    delete m_socket;
    if (m_auth)
        delete m_auth;
    delete m_inPacket;
}

bool Session::update(uint32 p_diff)
{
    switch (m_connectionstate)
    {
    case 0: // not connected
        break;
    case 1: // first tick
        m_connectionstate = 2;
        break;
    case 2: // during auth
    {
        m_inactiveTimer += p_diff;
        if (m_inactiveTimer > MAX_INACTIVITY)
        {
            log("ssid%u: kicking due to inactive\n", getSSID());
            return false;
        }

        if (m_socket->recv(m_inPacket))
        {
            if (m_inPacket->getOpcode() != CMSG_AUTH)
                break;
            if (!handleAuth())
                return false;
        }
        break;
    }
    case 3: // post auth
    {
        if (m_inactiveTimer < PING_DELAY && m_inactiveTimer + p_diff >= PING_DELAY)
        {
            IOPacket l_ping;
            l_ping.reset(1, MSG_PING_PONG);
            l_ping << uint8(1);
            sendPacket(&l_ping);
        }
        m_inactiveTimer += p_diff;
        while (m_socket->recv(m_inPacket))
        {
            processPacket();
            m_inactiveTimer = 0;
        }
        if (m_inactiveTimer > MAX_INACTIVITY)
        {
            m_socket->close_socket();
            log("ssid%u: kicking due to inactive\n", getSSID());
        }
        break;
    }
    }

    if (!m_socket->isConnected())
    {
        log("ssid%u: quiting\n", m_ssid);
        if (!m_username.empty())
        {
            IOPacket l_left;
            l_left.reset(m_username.size() + 1, SMSG_USER_LEFT);
            l_left << m_username;
            m_manager->sendToAll(&l_left, getSSID());
        }

        return false;
    }

    return true;
}

void Session::sendPacket(IOPacket* p_out)
{
    m_socket->send(p_out);
}

void Session::kick()
{
    m_socket->close_socket();
}

bool Session::handleAuth()
{
    m_inactiveTimer = 0;
    uint8 result = m_auth->handleAuth(m_inPacket);
    m_socket->send(m_inPacket);
    if (result == AUTH_OK)
    {
        m_socket->setKey(m_auth->getKey()); // from now communication is encrypted
        m_username = m_auth->getUsername();
        m_accid = m_auth->getAccid();
        log("ssid%u: user \"%s\" authorized; acc id %u\n",
            m_ssid, m_username.c_str(), m_accid);
        memset(m_auth, 0x00, sizeof(SAuthHandler));
        delete m_auth;
        m_auth = nullptr;
        m_connectionstate = 3;

        IOPacket l_joined;
        l_joined.reset(m_username.size() + 1, SMSG_USER_JOINED);
        l_joined << m_username;
        m_manager->sendToAll(&l_joined);
        if (!m_manager->getBoard()->addPlayer(getAccId(), m_username))
        {
            log("ssid%u: cannot add player to board, account %u\n", m_ssid, m_accid);
        }

    }
    else if (result != AUTH_NOT_YET)
    {
        // handle problems
        log("ssid%u: user \"%s\" unauthorized, result %u\n",
            m_ssid, m_auth->getUsername().c_str(), result);
        return false;
    }
    return true;
}

void Session::processPacket()
{
    switch (m_inPacket->getOpcode())
    {
    case MSG_TEXT_MESSAGE:
    {
        std::string l_what;
        *m_inPacket >> l_what;
        if (l_what.size() > 0 && l_what[0] == '/')
            m_manager->getBoard()->playerMove(getAccId(), l_what);
        else
        {
            m_inPacket->reset((m_username.size() + l_what.size() + 2), MSG_TEXT_MESSAGE);
            *m_inPacket << m_username << l_what;
            m_manager->sendToAll(m_inPacket);
        }
        break;
    }
    case MSG_PING_PONG:
    {
        break;
    }
    }
}

uint32 Session::NewSSID()
{
    static uint32 id = 1;
    return id++;
}

