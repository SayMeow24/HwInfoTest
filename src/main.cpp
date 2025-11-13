#include "HardwareInfoProvider.h"

int main()
{
    HardwareInfoProvider provider;
    ArgentumDevice dev = provider.getDeviceInfo();

    HardwareInfoProvider::printDeviceInfo(dev);

    return 0;
}
