#pragma once
#include "Defines.h"

class Display
{
public:
    Display() : m_mode(1) {}
    void setMode(uint8 mode) {m_mode=mode;}
    void showHand(uint8 count, uint32 bs);
    void showReveal(uint8 count, std::pair<std::string,uint32> players[]);
private:
    uint8 m_mode;
};
