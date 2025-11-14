#include "HardwareInfoProvider.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

std::filesystem::path getExecutableDir()
{
#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();
#else
    char buffer[4096];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::filesystem::path(buffer).parent_path();
    }
    return std::filesystem::current_path();
#endif
}

void saveJsonNearExe(const std::string& jsonString)
{
    auto exeDir = getExecutableDir();
    auto file = exeDir / "argentum_device.json";

    try {
        std::ofstream out(file);
        out << jsonString;
        out.close();
        std::cout << "\nJSON saved to: " << file.string() << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to save JSON: " << e.what() << std::endl;
    }
}

int main()
{
    HardwareInfoProvider provider;

    ArgentumDevice dev = provider.getDeviceInfo();

    HardwareInfoProvider::printDeviceInfo(dev);

    std::string json = HardwareInfoProvider::toJSON(dev);

    saveJsonNearExe(json);

    return 0;
}
