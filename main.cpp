#include <iostream>
#include "network_sheduler.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 0;
    }

    NetworkSheduler *sheduler = new NetworkSheduler(argv[1]);

    sheduler->start();

    getchar();

    return 0;
}
