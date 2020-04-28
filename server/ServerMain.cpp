#include "ListenerSocket.h"
#include "Logging.h"
#include "Session.h"
#include "BaseSocket.h"		// network_status
#include "Util.h"			// MessageQueue, getTimeDiff
#include "SessionMgr.h"
#include <thread>

#define WORLD_SLEEP 100

void listenerMain(SessionMgr* p_manager)
{
    ListenerSocket me;
    int started = me.start_listening(DEFAULT_PORT);
    if (started != 0)
    {
        log("could not start listening, error %u",errno);
        return;
    }

    if (me.isListening())
        log("listening...");

    while (true)
    {
        int catched = me.get_connection(1000);
        if (catched != -1)
            p_manager->QueueSocket(catched);
    }
}

int main()
{
    srand(time(NULL));
    SessionMgr l_sessMgr;

    log("Starting server");
    std::thread listenerThr(listenerMain, &l_sessMgr);
    listenerThr.detach();

    uint32 lastTickTime;
    getTimeDiff(lastTickTime, true);
    while (true) // main loop
    {
        uint32 diff = getTimeDiff(lastTickTime, true);
        l_sessMgr.Update(diff);

        diff = getTimeDiff(lastTickTime, false); // amount of ms it took to process everything
        if (diff < WORLD_SLEEP)
            std::this_thread::sleep_for(std::chrono::milliseconds(WORLD_SLEEP - diff));
    }

    return 0;
}
