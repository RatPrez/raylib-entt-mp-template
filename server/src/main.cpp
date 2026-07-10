#include <cstdint>
#include <cstdlib>

#include "core/Server.hpp"

int main(int argc, char **argv)
{
    uint32_t tickRate = 20;
    if (argc > 1) {
        tickRate = (uint32_t)atoi(argv[1]);
        if (tickRate == 0) {
            tickRate = 20;
        }
    }

    Server server(tickRate);
    server.run();
    return 0;
}
