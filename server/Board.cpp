#include "Board.h"
#include "SessionMgr.h"
#include "ByteBuffer.h"
#include <sstream>
#include <bitset>
#include <algorithm>
pstate::pstate() : count(0)
{
    memset(card,0,5);
}

Board::Board(SessionMgr* p_sm) : m_state(STATE_WAIT), m_sm(p_sm)
{
}

void putCard(std::ostream& ss, uint8 card)
{
    if (card>24) return;
    const char* v[] = {"A ", "K ", "Q ", "J ", "10 ", "9 "};
    const char* c[] = {"Pik", "Kier", "Karo", "Trefl"};
    ss << v[card%6] << c[card/6];
}

bool Board::addPlayer(uint32 p_player, std::string p_name)
{
    if (!p_player) return false;
    auto itr = m_playerstates.find(p_player);
    if (itr != m_playerstates.end() || m_state == STATE_WAIT)
    {
        m_playerstates[p_player].name = p_name;
        sendState(p_player, true);
    }
    sendGlobalState(p_player);
    return true;
}

void Board::playerDisconnected(uint32 p_player)
{
    if (!p_player) return;
    if (m_state == STATE_WAIT)
        m_playerstates.erase(p_player);
}

void Board::playerMove(uint32 p_player, std::string text)
{
    if (text == "/status")
    {
        sendGlobalState(p_player);
        if (m_state == STATE_PLAY) sendState(p_player);
        return;
    }

    if (!p_player) return;

    if (text == "/reset")
    {
        sendToAll("game reset by " + m_playerstates[p_player].name);
        Reset();
        sendGlobalState(0,true);
        return;
    }

    if (text == "/reveal" && m_state == STATE_PLAY)
    {
        sendToAll("cards revealed by " + m_playerstates[p_player].name);
        reveal();
        return;
    }

    if (text == "/add" && m_state == STATE_REVEAL)
    {
        m_playerstates[p_player].count = (m_playerstates[p_player].count +1)%6;
        std::ostringstream ss;
        ss <<"new card for " << m_playerstates[p_player].name << " (" << int(m_playerstates[p_player].count) << ")";
        sendToAll(ss.str());
        return;
    }

    if (text == "/play" && m_state != STATE_PLAY)
    {
        if (m_state == STATE_WAIT)
        {
            init();
            return;
        }
        deal();
        return;
    }
}

void Board::sendToAll(std::string str)
{
    IOPacket packet;
    packet.reset(str.length()+1,SMSG_BOARD_STATE);
    packet << str;
    m_sm->sendToAll(&packet);
}

void Board::sendGlobalState(uint32 p_player, bool toall)
{
    IOPacket packet;
    std::string gs = getGlobalState();
    packet.reset(gs.length()+1,SMSG_BOARD_STATE);
    packet << gs;
    if (toall)
        m_sm->sendToAll(&packet);
    else
        m_sm->sendTo(&packet,p_player);
}

void Board::sendState(uint32 p_player, bool hello)
{
    const char hello_world[] =
         "\n.------..------..------..------.\n"
         "|K.--. ||N.--. ||P.--. ||B.--. |\n"
         "| :/\\: || :(): || :/\\: || :(): |\n"
         "| :\\/: || ()() || (__) || ()() |\n"
         "| '--'K|| '--'N|| '--'P|| '--'B|\n"
         "`------'`------'`------'`------'\n";
    std::ostringstream ss;
    //ss << m_playerstates[p_player];
    //std::ostream& operator<<(std::ostream& ss, pstate& ps)
    if (hello) ss << hello_world;
    ss << "you have " << int(m_playerstates[p_player].count) << " card(s)";
    if (m_state == STATE_PLAY)
    {
        ss << ": ";
        for (uint8 i =0; i< m_playerstates[p_player].count && i < 5; ++i)
        {
            if (i) ss << ", ";
            putCard(ss, m_playerstates[p_player].card[i]);
        }
    }

    IOPacket packet;
    packet.reset(ss.str().length()+1,SMSG_BOARD_STATE);
    packet << ss.str();
    m_sm->sendTo(&packet,p_player);
}

std::string Board::getGlobalState()
{
    std::ostringstream ss;
    if (m_state == STATE_WAIT)
    {
        ss << "Waiting for start, " << m_playerstates.size() << " players joined";
        return ss.str();
    }
    ss << (m_state == STATE_PLAY ? "Game in play, " : "Waiting for next round, " );
    ss << m_playerstates.size() << " players";
    int cards=0;
    for (auto i : m_ingame)
    {
        ss << "\n" << m_playerstates[i].name << " : " << int(m_playerstates[i].count) << " card(s)";
        cards += m_playerstates[i].count;
    }
    ss << "\n" << cards << " cards total";
    return ss.str();
}

void Board::Reset()
{
    m_state = STATE_WAIT;
    m_playerstates.clear();
    m_sm->kickAll();
    m_ingame.clear();
}

void Board::init()
{
    m_state = STATE_REVEAL;
    for (auto& itr : m_playerstates)
    {
        if (itr.first == 0) continue;
        m_ingame.push_back(itr.first);
        itr.second.count =1;
    }
    std::random_shuffle(m_ingame.begin(), m_ingame.end());
    sendGlobalState(0,true);
}

void Board::reveal()
{
    std::bitset<24> bs;
    std::ostringstream ss;
    m_state = STATE_REVEAL;
    for (auto i : m_ingame)
    {
        ss << m_playerstates[i].name << " : ";
        for (uint8 j =0; j< m_playerstates[i].count && j < 5; ++j)
        {
            if (j) ss << ", ";
            putCard(ss, m_playerstates[i].card[j]);
            bs.set(m_playerstates[i].card[j]);
        }
        ss << "\n";
    }
    ss << "total:\n";
    for (int i =0; i< 24;++i)
    {
        switch (i)
        {
            case 0:
                ss << "Piki:   "; break;
            case 6:
                ss << "Kiery:  "; break;
            case 12:
                ss << "Kara:   "; break;
            case 18:
                ss << "Trefle: "; break;
        }
        switch(i%6)
        {
            case 0:
                ss << (bs.test(i) ? "A " : "  "); break;
            case 1:
                ss << (bs.test(i) ? "K " : "  "); break;
            case 2:
                ss << (bs.test(i) ? "Q " : "  "); break;
            case 3:
                ss << (bs.test(i) ? "J " : "  "); break;
            case 4:
                ss << (bs.test(i) ? "10 " : "   "); break;
            case 5:
                ss << (bs.test(i) ? "9\n" : "\n"); break;
        }
    }
    sendToAll(ss.str());
}

void Board::deal()
{
    m_state = STATE_PLAY;
    std::vector<uint8> deck(24);
    for (int i = 0; i<24;++i)
        deck[i]=i;
    std::random_shuffle(deck.begin(), deck.end());
    for (auto i : m_ingame)
    {
        for (int j =0 ; j < m_playerstates[i].count; ++j)
        {
            m_playerstates[i].card[j] = deck.back();
            deck.pop_back();
        }
        sendState(i);
    }
    sendGlobalState(0,true);
}
