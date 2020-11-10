#include "SessionMgr.h"
#include "MainSocket.h"
#include "MySha.h"
#include "Util.h"
#include "Session.h"
#include "Board.h"

void SessionMgr::loadFromTxt(const char* file)
{
    m_accounts.clear();
    FILE* pFile = fopen(file, "r");
    if (!pFile)
        return;

    while (!feof(pFile))
    {
        AccountData ad;
        char digits[60];
        char cname[20];
        if (!fscanf(pFile, "%u ", &ad.Id))
            break;
        if (!fscanf(pFile, "%s ", cname))
            break;
        if (!fscanf(pFile, "%60c\n", digits))
            break;

        ad.name = std::string(cname);
        for (uint8 i = 0; i < 20; i++)
            ad.passHash[i] = (digits[3 * i] - '0') * 100 + (digits[3 * i + 1] - '0') * 10 + (digits[3 * i + 2] - '0');

        m_accounts.push_back(ad);
    }
    fclose(pFile);
}

AccountData* SessionMgr::getUserData(std::string name)
{
    std::list<AccountData>::iterator itr;
    for (itr = m_accounts.begin(); itr != m_accounts.end(); itr++)
    {
        if (name == itr->name)
            break;
    }
    if (itr != m_accounts.end())
        return &(*itr);

    // test ones
    if (isValidName(name))
    {
        AccountData ad;
        ad.Id = 0;
        ad.name = "T_" + name;
        MySha sha;
        sha.Initialize();
        sha.UpdateData(name);
        sha.UpdateData(":");
        sha.UpdateData(name);
        sha.Finalize();
        memcpy(ad.passHash, sha.GetDigest(), 20);
        m_accounts.push_back(ad);
        return &(m_accounts.back());
    }

    return NULL;
}

bool SessionMgr::isValidName(std::string name) const
{
    return name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") == std::string::npos;
}


SessionMgr::SessionMgr()
{
    m_socketsQueue = MessageQueue::getNewQueue();
    loadFromTxt("users.txt");
    m_board = new Board(this);
}

SessionMgr::~SessionMgr()
{
    delete m_socketsQueue;
}

void SessionMgr::Update(uint32 diff)
{
    int newSocket;
    if (m_socketsQueue->pop(newSocket))
    {
        m_sessionList.push_back(new Session(newSocket, this));
    }

    for (std::list<Session*>::iterator itr = m_sessionList.begin(); itr != m_sessionList.end();)
    {
        if (!(*itr)->update(diff))
        {
            m_board->playerDisconnected((*itr)->getAccId());
            delete *itr;
            itr = m_sessionList.erase(itr);
        }
        else
            itr++;
    }
}

void SessionMgr::QueueSocket(int socket)
{
    m_socketsQueue->push(socket);
}

void SessionMgr::sendToAll(IOPacket* what, uint32 except)
{
    for (std::list<Session*>::iterator itr = m_sessionList.begin(); itr != m_sessionList.end(); itr++)
    {
        if (except && (*itr)->getAccId() == except)
            continue;
        (*itr)->sendPacket(what);
    }
}

void SessionMgr::sendTo(IOPacket* what, uint32 who)
{
    for (std::list<Session*>::iterator itr = m_sessionList.begin(); itr != m_sessionList.end(); itr++)
    {
        if ((*itr)->getAccId() == who)
            (*itr)->sendPacket(what);
    }
}

void SessionMgr::kickAll()
{
    for (std::list<Session*>::iterator itr = m_sessionList.begin(); itr != m_sessionList.end(); itr++)
    {
        (*itr)->kick();
    }
}

bool SessionMgr::isAdmin(uint32 user)
{
    return (user < 10);
}

void SessionMgr::reload()
{
    loadFromTxt("users.txt");
}
