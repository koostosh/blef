#pragma once
#include "Defines.h"

class SessionMgr;

struct pstate
{
    uint8 card[5];
    uint8 count;
    std::string name;
    pstate();
};

class Board
{
    public:
        Board(SessionMgr* p_sm);

        void playerDisconnected(uint32 p_player);
        void playerMove(uint32 p_player, std::string text);
        bool addPlayer(uint32 p_player, std::string p_name);
    private:
        void init();
        void deal();
        void reveal();


        void Reset();
        void sendState(uint32 p_player, bool hello =false);
        std::string getGlobalState();
        void sendGlobalState(uint32 p_player, bool toall= false);
        void sendToAll(std::string p_str);

        std::map<uint32, pstate> m_playerstates;
        std::vector<uint32> m_ingame;
        SessionMgr* m_sm;
        enum {STATE_WAIT, STATE_PLAY, STATE_REVEAL} m_state;
};
