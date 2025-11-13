#include "HardwareInfoProvider.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    bool jsonMode = false;

    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--json" || arg == "-j") {
            jsonMode = true;
        }
    }

    HardwareInfoProvider provider;
    ArgentumDevice dev = provider.getDeviceInfo();

    if (jsonMode) {
        std::string json = HardwareInfoProvider::toJSON(dev);
        std::cout << json << std::endl;
    }
    else {
        HardwareInfoProvider::printStructure(dev);
    }

    return 0;
}
