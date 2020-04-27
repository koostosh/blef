#pragma once
#include "Defines.h"

uint32 rand32();
uint32 urand(uint32 min, uint32 max);
uint32 getTimeDiff(uint32& old, bool change);
std::string hashFrom(std::string user, std::string pass);

class MessageQueue
{
public:
    virtual bool pop(int& val) { return false; };
    virtual void push(int val) {};
    static MessageQueue* getNewQueue();
protected:
    MessageQueue() {};
};
