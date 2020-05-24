#include "Display.h"
#include "Format.h"
#include <sstream>


void putHand(std::ostream& ss, uint32 bs)
{
    const char* v[] = {"A ", "K ", "Q ", "J ", "10 ", "9 "};
    const char* c[] = {"Pik", "Kier", "Karo", "Trefl"};
    bool any = false;
    for (int i = 0; i < 24; ++i)
    {
        if (bs & (1 << i))
        {
            if (any)
                ss << ", ";
            any = true;
            ss << v[i%6] << c[i/6];
        }
    }
}

void putHand_1(std::ostream& ss, uint32 bs)
{
    const char* v[] = {"A", "K", "Q", "J", "10", "9"};
    const char* c[] = {"\u2660", "\u2665", "\u2666", "\u2663"};

    bool any = false;
    for (int i = 0; i < 24; ++i)
    {
        if (bs & (1 << i))
        {
            if (any)
                ss << ", ";
            any = true;
            ss << v[i%6] << c[i/6];
        }
    }
}

void putHand_2(std::ostream& ss, uint32 bs, uint8 count)
{
    const char* v[] = {"A", "K", "Q", "J", "X", "9"};
    const char* c[] = {"\u2660", "\u2665", "\u2666", "\u2663"};
    uint8* cards = new uint8[count];
    uint8 j =0;
    for (int i = 0; i < 24 && j < count; ++i)
    {
        if (bs & (1 << i))
        {
            cards[j++] = i;
        }
    }

    for (j=0; j< count; ++j)
        ss << "┏━━━━━┓";
    ss << std::endl;
    for (j=0; j< count; ++j)
        ss << "┃" << v[cards[j]%6] << "    ┃";
    ss << std::endl;
    for (j=0; j< count; ++j)
        ss << "┃  " << c[cards[j]/6] << "  ┃";
    ss << std::endl;
    for (j=0; j< count; ++j)
        ss << "┃    " << v[cards[j]%6] << "┃";
    ss << std::endl;
    for (j=0; j< count; ++j)
        ss << "┗━━━━━┛";

    delete[] cards;
}

void putHand_3(std::ostream& ss, uint32 bs, uint8 count)
{
    const char* v[] = {"A", "K", "Q", "J", "X", "9"};
    const char* c[] = {"\u2660", "\u2665", "\u2666", "\u2663"};
    uint8* cards = new uint8[count];
    uint8 j =0;
    for (int i = 0; i < 24 && j < count; ++i)
    {
        if (bs & (1 << i))
        {
            cards[j++] = i;
        }
    }

    for (j=0; j< count; ++j)
        ss << "┏━━━━━━┓";
    ss << std::endl;
    for (j=0; j< count; ++j)
        ss << "┃" << v[cards[j]%6] << "     ┃";
    ss << std::endl;
    for (j=0; j< count; ++j)
        ss << "┃" << c[cards[j]/6] << "     ┃";
    ss << std::endl;
    for (j=0; j< count; ++j)
        ss << "┃     " << c[cards[j]/6] << "┃";
    ss << std::endl;
    for (j=0; j< count; ++j)
        ss << "┃     " << v[cards[j]%6] << "┃";
    ss << std::endl;
    for (j=0; j< count; ++j)
        ss << "┗━━━━━━┛";

    delete[] cards;
}
void Display::showHand(uint8 count, uint32 bs)
{
    printf(MSG_STATUS("You have %u card(s)"), count);
    if (bs == 0) return;
    std::ostringstream ss;
    if (m_mode == 1)
        putHand_1(ss,bs);
    else if (m_mode ==2)
        putHand_2(ss,bs,count);
    else if (m_mode ==3)
        putHand_3(ss,bs,count);
    else
        putHand(ss,bs);
    printf(MSG_STATUS("%s"), ss.str().c_str());
}


void Display::showReveal(uint8 count, std::pair<std::string,uint32> players[])
{
    std::ostringstream ss;
    for (int i =0; i < count; ++i)
    {
        ss << players[i].first <<" : ";
        if (m_mode != 0)
            putHand_1(ss,players[i].second);
        else
            putHand(ss, players[i].second);
        if (i < count-1) ss << std::endl;
    }
    printf(MSG_STATUS("%s"),ss.str().c_str());
}
