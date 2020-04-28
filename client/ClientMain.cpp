#include "Session.h"
#include <future>
#include <iostream>

int main(int argc, char* argv[])
{
    Session l_sess;
    
    while (true)
    {
        std::future<std::string> in = std::async(std::launch::async, [](void){ std::string ret; std::cin >> ret; return ret;});
        while (true)
        {
            l_sess.Update();
            if (in.wait_for(std::chrono::milliseconds(100)) == std::future_status::ready)
            {
                l_sess.input(in.get());
                break;
            }
        }
    }
    return 0;
}
