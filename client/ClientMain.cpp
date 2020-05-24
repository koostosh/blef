#include "Format.h"
#include "Session.h"
#include <future>
#include <iostream>

int main(int argc, char* argv[])
{
    Session l_sess;

    if (argc > 3)
        l_sess.InitData(argv[1], argv[2], argv[3]);
    while (true)
    {
        std::future<std::string> in = std::async(std::launch::async, [](void){ std::string ret; std::getline(std::cin, ret); return ret;});
        while (true)
        {
            l_sess.Update();
            if (in.wait_for(std::chrono::milliseconds(100)) == std::future_status::ready)
            {
                if (std::cin.eof()) {
                    std::cout << MSG_INFO_("End of input, Bye!") << std::endl;
                    return 0;
                }

                l_sess.input(in.get());
                break;
            }
        }
    }

    return 0;
}
