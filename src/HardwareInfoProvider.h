#pragma once

#include "../HardwareInfo.h"

#include <string>

class HardwareInfoProvider
{
public:
    HardwareInfoProvider() = default;
    ~HardwareInfoProvider() = default;

    // Зібрати повну інформацію
    ArgentumDevice getDeviceInfo() const;

    // Вивід у консоль
    static void printDeviceInfo(const ArgentumDevice& dev);

    // JSON як std::string
    static std::string toJSON(const ArgentumDevice& dev);

private:
    // Helpers
    static std::string formatMBToGB(uint64_t mb);
    static std::string escapeJson(const std::string& s);

#ifdef _WIN32
    void fillOSWindows(ArgentumDevice& dev) const;
    void fillCPUWindows(ArgentumDevice& dev) const;
    void fillRAMWindows(ArgentumDevice& dev) const;
    void fillGPUWindows(ArgentumDevice& dev) const;
    void fillDisksWindows(ArgentumDevice& dev) const;
#endif

#ifdef __linux__
    void fillOSLinux(ArgentumDevice& dev) const;
    void fillCPULinux(ArgentumDevice& dev) const;
    void fillRAMLinux(ArgentumDevice& dev) const;
    void fillGPULinux(ArgentumDevice& dev) const;
    void fillDisksLinux(ArgentumDevice& dev) const;
#endif
};
