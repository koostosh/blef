#pragma once
#include "Defines.h"

class MessageQueue;
class Session;
class IOPacket;
class Board;

struct AccountData
{
    uint32      Id; // equal zero means guest
    std::string name;
    uint8       passHash[20];
};
class SessionMgr
{
public:
    SessionMgr();
    ~SessionMgr();

    void Update(uint32 diff);
    void QueueSocket(int socket);
    AccountData* getUserData(std::string name);
    void sendToAll(IOPacket* what, uint32 except = 0);
    void sendTo(IOPacket* what, uint32 who);
    void kickAll();
    Board* getBoard(){ return m_board; };

    bool isAdmin(uint32 user);
    void reload();
private:
    bool isValidName(std::string name) const;

    std::list<Session*> m_sessionList;
    MessageQueue*       m_socketsQueue;
    Board*              m_board;

    // account management
    void loadFromTxt(const char* file);
    std::list<AccountData> m_accounts;
};
