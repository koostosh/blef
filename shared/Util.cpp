#include "Util.h"
#include "MersenneTwister.h"
#include "MySha.h"
#include <time.h>
#include <mutex>

static MTRand mtrand;
uint32 urand(uint32 min, uint32 max)
{
    return mtrand.randInt(max - min) + min;
}

uint32 rand32()
{
    return mtrand.randInt();
}

uint32 getTimeDiff(uint32& old, bool change)
{
    uint32 newTime = clock() * 1000 / CLOCKS_PER_SEC;
    uint32 diff = newTime - old;
    if (change)
        old = newTime;
    return diff;
}

std::string hashFrom(std::string user, std::string pass)
{
    MySha sha;
    sha.Initialize();
    sha.UpdateData(user);
    sha.UpdateData(":");
    sha.UpdateData(pass);
    sha.Finalize();

    std::string ret;
    for (uint8 i = 0; i < 20; i++)
    {
        char digits[4];
        sprintf(digits, "%03u", sha.GetDigest()[i]);
        ret += digits;
    }
    return ret;
}


class MessageQueueImpl : public MessageQueue
{
public:
    MessageQueueImpl() {};

    bool pop(int& val)
    {
        bool ret;
        m_mutex.lock();
        if (m_list.empty())
        {
            ret = false;
        }
        else
        {
            ret = true;
            val = *m_list.begin();
            m_list.pop_front();
        }
        m_mutex.unlock();
        return ret;
    }

    void push(int val)
    {
        m_mutex.lock();
        m_list.push_back(val);
        m_mutex.unlock();
    }
private:
    std::list<int> m_list;
    std::mutex m_mutex;
};

MessageQueue* MessageQueue::getNewQueue()
{
    return new MessageQueueImpl;
}
