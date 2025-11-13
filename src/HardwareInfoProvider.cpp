#include "HardwareInfoProvider.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <VersionHelpers.h>
#include <sysinfoapi.h>
#include <dxgi.h>
#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "dxgi.lib")
#endif

#ifdef __linux__
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#endif

// ========================== Helpers ==========================

std::string HardwareInfoProvider::diskTypeToString(DiskType type)
{
    switch (type) {
    case DiskType::HDD:       return "HDD";
    case DiskType::SSD:       return "SSD";
    case DiskType::External:  return "External";
    case DiskType::Removable: return "Removable";
    default:                  return "Unknown";
    }
}

std::string HardwareInfoProvider::formatBytesMB(uint64_t mb)
{
    double gb = mb / 1024.0;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << gb << " GB";
    return oss.str();
}

std::string HardwareInfoProvider::escapeJson(const std::string& s)
{
    std::ostringstream o;
    for (char c : s) {
        switch (c) {
        case '\"': o << "\\\""; break;
        case '\\': o << "\\\\"; break;
        case '\b': o << "\\b";  break;
        case '\f': o << "\\f";  break;
        case '\n': o << "\\n";  break;
        case '\r': o << "\\r";  break;
        case '\t': o << "\\t";  break;
        default:
            if (static_cast<unsigned char>(c) < 0x20) {
                o << "\\u"
                    << std::hex << std::setw(4) << std::setfill('0')
                    << (int)(unsigned char)c;
            }
            else {
                o << c;
            }
        }
    }
    return o.str();
}

// ========================== getDeviceInfo ==========================

ArgentumDevice HardwareInfoProvider::getDeviceInfo() const
{
    ArgentumDevice dev;

#ifdef _WIN32
    fillOSInfoWindows(dev);
    fillCPUWindows(dev);
    fillRAMWindows(dev);
    fillGPUWindows(dev);
    fillDisksWindows(dev);
#elif defined(__linux__)
    fillOSInfoLinux(dev);
    fillCPULinux(dev);
    fillRAMLinux(dev);
    fillGPULinux(dev);
    fillDisksLinux(dev);
#else
    dev.os = "Unknown OS";
    dev.platform = "Unknown";
#endif

    // Сумарна статистика по дисках
    uint64_t total = 0, free = 0;
    for (const auto& d : dev.disks) {
        total += d.total_mb;
        free += d.free_mb;
    }
    if (total > 0) {
        dev.total_disk_mb = total;
        dev.free_disk_mb = free;
        dev.used_disk_mb = total - free;
        dev.disk_usage_percent =
            (double)(total - free) * 100.0 / (double)total;
    }

    if (!dev.gpus.empty()) {
        dev.gpu_count = static_cast<uint32_t>(dev.gpus.size());
    }

    return dev;
}

// ========================== printStructure ==========================

void HardwareInfoProvider::printStructure(const ArgentumDevice& device)
{
    std::cout << "=== ArgentumDevice Structure ===\n\n";

    std::cout << "Platform: " << device.platform.value_or("Unknown") << "\n\n";

    std::cout << "OS:\n";
    std::cout << "  Name: " << device.os << "\n";
    std::cout << "  Kernel: " << device.os_kernel.value_or("Unknown") << "\n";
    std::cout << "  Architecture: " << device.os_arch.value_or("Unknown") << "\n\n";

    std::cout << "CPU:\n";
    std::cout << "  Model: " << device.cpu_model.value_or("Unknown") << "\n";
    std::cout << "  Cores: " << device.cpu_cores << "\n";
    if (device.cpu_frequency_mhz) {
        std::cout << "  Frequency: "
            << device.cpu_frequency_mhz.value()
            << " MHz\n";
    }
    std::cout << "\n";

    std::cout << "RAM:\n";
    std::cout << "  Total: " << device.ram_mb << " MB ("
        << formatBytesMB(device.ram_mb) << ")\n";
    if (device.ram_available_mb)
        std::cout << "  Available: " << device.ram_available_mb.value() << " MB\n";
    if (device.ram_used_mb)
        std::cout << "  Used: " << device.ram_used_mb.value() << " MB\n";
    if (device.ram_usage_percent)
        std::cout << "  Usage: "
        << std::fixed << std::setprecision(1)
        << device.ram_usage_percent.value() << "%\n";
    std::cout << "\n";

    std::cout << "GPU:\n";
    if (device.gpu_count)
        std::cout << "  Count: " << device.gpu_count.value() << "\n\n";

    for (size_t i = 0; i < device.gpus.size(); ++i) {
        const auto& g = device.gpus[i];
        std::cout << "  GPU " << (i + 1) << ": " << g.model << "\n";
        if (g.vram_mb)
            std::cout << "    VRAM Total: " << g.vram_mb.value() << " MB ("
            << formatBytesMB(g.vram_mb.value()) << ")\n";
        if (g.vram_used_mb)
            std::cout << "    VRAM Used:  " << g.vram_used_mb.value() << " MB\n";
        if (g.vram_free_mb)
            std::cout << "    VRAM Free:  " << g.vram_free_mb.value() << " MB\n";
        if (g.vram_usage_percent)
            std::cout << "    Usage: "
            << std::fixed << std::setprecision(1)
            << g.vram_usage_percent.value() << "%\n";
        std::cout << "\n";
    }

    std::cout << "Disks:\n";
    for (const auto& d : device.disks) {
        std::cout << "  " << d.mount_point << " (" << d.filesystem << ")\n";
        std::cout << "    Type: " << diskTypeToString(d.type) << "\n";
        std::cout << "    Size: " << d.total_mb << " MB ("
            << formatBytesMB(d.total_mb) << ")\n";
        std::cout << "    Free: " << d.free_mb << " MB\n";
        std::cout << "    Used: " << d.used_mb << " MB\n";
        std::cout << "    Usage: "
            << std::fixed << std::setprecision(1)
            << d.usage_percent << "%  (Free: "
            << d.free_percent << "%)\n\n";
    }

    if (device.total_disk_mb) {
        std::cout << "Total Disks:\n";
        std::cout << "  Total: " << device.total_disk_mb.value()
            << " MB (" << formatBytesMB(device.total_disk_mb.value()) << ")\n";
        if (device.free_disk_mb)
            std::cout << "  Free: " << device.free_disk_mb.value() << " MB\n";
        if (device.used_disk_mb && device.disk_usage_percent)
            std::cout << "  Used: " << device.used_disk_mb.value() << " MB ("
            << std::fixed << std::setprecision(1)
            << device.disk_usage_percent.value() << "%)\n";
    }

    std::cout << "\nPrimary Disk Type: "
        << diskTypeToString(device.primary_disk_type) << "\n";
}

// ========================== toJSON ==========================

std::string HardwareInfoProvider::toJSON(const ArgentumDevice& dev)
{
    std::ostringstream out;
    out << "{\n";

    auto indent = [](int n) {
        return std::string(n, ' ');
        };

    int ind = 2;

    out << indent(ind) << "\"os\": \"" << escapeJson(dev.os) << "\",\n";

    if (dev.platform)
        out << indent(ind) << "\"platform\": \"" << escapeJson(*dev.platform) << "\",\n";
    if (dev.os_kernel)
        out << indent(ind) << "\"os_kernel\": \"" << escapeJson(*dev.os_kernel) << "\",\n";
    if (dev.os_arch)
        out << indent(ind) << "\"os_arch\": \"" << escapeJson(*dev.os_arch) << "\",\n";

    if (dev.cpu_model)
        out << indent(ind) << "\"cpu_model\": \"" << escapeJson(*dev.cpu_model) << "\",\n";

    out << indent(ind) << "\"cpu_cores\": " << dev.cpu_cores << ",\n";

    if (dev.cpu_frequency_mhz)
        out << indent(ind) << "\"cpu_frequency_mhz\": "
        << dev.cpu_frequency_mhz.value() << ",\n";

    out << indent(ind) << "\"ram_mb\": " << dev.ram_mb << ",\n";

    if (dev.ram_available_mb)
        out << indent(ind) << "\"ram_available_mb\": "
        << dev.ram_available_mb.value() << ",\n";
    if (dev.ram_used_mb)
        out << indent(ind) << "\"ram_used_mb\": "
        << dev.ram_used_mb.value() << ",\n";
    if (dev.ram_usage_percent)
        out << indent(ind) << "\"ram_usage_percent\": "
        << dev.ram_usage_percent.value() << ",\n";

    // GPUs
    out << indent(ind) << "\"gpus\": [\n";
    for (size_t i = 0; i < dev.gpus.size(); ++i) {
        const auto& g = dev.gpus[i];
        out << indent(ind + 2) << "{\n";
        out << indent(ind + 4) << "\"model\": \"" << escapeJson(g.model) << "\"";
        if (g.vram_mb) {
            out << ",\n" << indent(ind + 4)
                << "\"vram_mb\": " << g.vram_mb.value();
        }
        if (g.vram_used_mb) {
            out << ",\n" << indent(ind + 4)
                << "\"vram_used_mb\": " << g.vram_used_mb.value();
        }
        if (g.vram_free_mb) {
            out << ",\n" << indent(ind + 4)
                << "\"vram_free_mb\": " << g.vram_free_mb.value();
        }
        if (g.vram_usage_percent) {
            out << ",\n" << indent(ind + 4)
                << "\"vram_usage_percent\": " << g.vram_usage_percent.value();
        }
        out << "\n" << indent(ind + 2) << "}";
        if (i + 1 < dev.gpus.size())
            out << ",";
        out << "\n";
    }
    out << indent(ind) << "],\n";

    if (dev.gpu_count)
        out << indent(ind) << "\"gpu_count\": " << dev.gpu_count.value() << ",\n";

    // Disks
    out << indent(ind) << "\"disks\": [\n";
    for (size_t i = 0; i < dev.disks.size(); ++i) {
        const auto& d = dev.disks[i];
        out << indent(ind + 2) << "{\n";
        out << indent(ind + 4) << "\"mount_point\": \""
            << escapeJson(d.mount_point) << "\",\n";
        out << indent(ind + 4) << "\"filesystem\": \""
            << escapeJson(d.filesystem) << "\",\n";
        out << indent(ind + 4) << "\"type\": \""
            << diskTypeToString(d.type) << "\",\n";
        out << indent(ind + 4) << "\"total_mb\": " << d.total_mb << ",\n";
        out << indent(ind + 4) << "\"free_mb\": " << d.free_mb << ",\n";
        out << indent(ind + 4) << "\"used_mb\": " << d.used_mb << ",\n";
        out << indent(ind + 4) << "\"usage_percent\": "
            << d.usage_percent << ",\n";
        out << indent(ind + 4) << "\"free_percent\": "
            << d.free_percent << "\n";
        out << indent(ind + 2) << "}";
        if (i + 1 < dev.disks.size())
            out << ",";
        out << "\n";
    }
    out << indent(ind) << "]";

    if (dev.total_disk_mb || dev.free_disk_mb ||
        dev.used_disk_mb || dev.disk_usage_percent)
    {
        out << ",\n";
        if (dev.total_disk_mb)
            out << indent(ind) << "\"total_disk_mb\": "
            << dev.total_disk_mb.value() << ",\n";
        if (dev.free_disk_mb)
            out << indent(ind) << "\"free_disk_mb\": "
            << dev.free_disk_mb.value() << ",\n";
        if (dev.used_disk_mb)
            out << indent(ind) << "\"used_disk_mb\": "
            << dev.used_disk_mb.value() << ",\n";
        if (dev.disk_usage_percent)
            out << indent(ind) << "\"disk_usage_percent\": "
            << dev.disk_usage_percent.value() << "\n";
        else {
            // прибираємо останню кому логічно
        }
    }
    else {
        out << "\n";
    }

    out << "\n}\n";
    return out.str();
}

// ========================== WINDOWS ==========================

#ifdef _WIN32

void HardwareInfoProvider::fillOSInfoWindows(ArgentumDevice& dev) const
{
    dev.platform = "Windows";

    OSVERSIONINFOEXA osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
#pragma warning(push)
#pragma warning(disable: 4996)
    if (GetVersionExA((OSVERSIONINFOA*)&osvi)) {
#pragma warning(pop)
        std::ostringstream oss;
        oss << "Windows " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion;
        dev.os = oss.str();

        std::ostringstream k;
        k << osvi.dwMajorVersion << "."
            << osvi.dwMinorVersion << "."
            << osvi.dwBuildNumber;
        dev.os_kernel = k.str();
    }
    else {
        dev.os = "Windows (unknown version)";
    }

    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    switch (si.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:
        dev.os_arch = "x86_64"; break;
    case PROCESSOR_ARCHITECTURE_INTEL:
        dev.os_arch = "x86";    break;
    default:
        dev.os_arch = "Unknown"; break;
    }
}

void HardwareInfoProvider::fillCPUWindows(ArgentumDevice& dev) const
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    dev.cpu_cores = si.dwNumberOfProcessors;

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        char name[256];
        DWORD size = sizeof(name);
        if (RegQueryValueExA(hKey, "ProcessorNameString", nullptr, nullptr,
            (LPBYTE)name, &size) == ERROR_SUCCESS)
        {
            dev.cpu_model = std::string(name);
        }

        DWORD mhz = 0;
        size = sizeof(mhz);
        if (RegQueryValueExA(hKey, "~MHz", nullptr, nullptr,
            (LPBYTE)&mhz, &size) == ERROR_SUCCESS)
        {
            dev.cpu_frequency_mhz = mhz;
        }
        RegCloseKey(hKey);
    }
}

void HardwareInfoProvider::fillRAMWindows(ArgentumDevice& dev) const
{
    MEMORYSTATUSEX ms = {};
    ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms)) {
        uint64_t total_mb = ms.ullTotalPhys / (1024ull * 1024ull);
        uint64_t avail_mb = ms.ullAvailPhys / (1024ull * 1024ull);
        dev.ram_mb = total_mb;
        dev.ram_available_mb = avail_mb;
        dev.ram_used_mb = total_mb - avail_mb;
        dev.ram_usage_percent =
            (double)(total_mb - avail_mb) * 100.0 / (double)total_mb;
    }
}

void HardwareInfoProvider::fillGPUWindows(ArgentumDevice& dev) const
{
    IDXGIFactory* pFactory = nullptr;
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory)))
        return;

    UINT i = 0;
    IDXGIAdapter* pAdapter = nullptr;
    while (pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC desc;
        if (SUCCEEDED(pAdapter->GetDesc(&desc))) {
            char name[256] = {};
            wcstombs(name, desc.Description, sizeof(name) - 1);
            std::string model(name);

            // ІГНОРУЄМО Microsoft Basic Render Driver
            if (model.find("Microsoft Basic Render Driver") != std::string::npos) {
                pAdapter->Release();
                ++i;
                continue;
            }

            GPUInfo gpu;
            gpu.model = model;

            uint64_t vram_mb = desc.DedicatedVideoMemory / (1024ull * 1024ull);
            if (vram_mb > 0)
                gpu.vram_mb = vram_mb;

            dev.gpus.push_back(gpu);
        }
        pAdapter->Release();
        ++i;
    }

    if (pFactory) pFactory->Release();
}

void HardwareInfoProvider::fillDisksWindows(ArgentumDevice& dev) const
{
    DWORD mask = GetLogicalDrives();
    if (mask == 0)
        return;

    char root[4] = "A:/";
    bool primarySet = false;

    for (int i = 0; i < 26; ++i) {
        if (!(mask & (1 << i))) continue;
        root[0] = 'A' + i;

        UINT type = GetDriveTypeA(root);
        if (type == DRIVE_NO_ROOT_DIR) continue;

        DiskInfo d;
        d.mount_point = root;
        d.filesystem = "NTFS";   // спрощено

        DiskType dt = DiskType::Unknown;
        if (type == DRIVE_FIXED)      dt = DiskType::SSD;       // не розрізняємо SSD/HDD
        else if (type == DRIVE_REMOVABLE) dt = DiskType::Removable;

        d.type = dt;

        ULARGE_INTEGER freeBytesAvailable, totalBytes, freeBytes;
        if (GetDiskFreeSpaceExA(root, &freeBytesAvailable, &totalBytes, &freeBytes)) {
            uint64_t total_mb = totalBytes.QuadPart / (1024ull * 1024ull);
            uint64_t free_mb = freeBytes.QuadPart / (1024ull * 1024ull);
            d.total_mb = total_mb;
            d.free_mb = free_mb;
            d.used_mb = total_mb - free_mb;
            if (total_mb > 0) {
                d.usage_percent = (double)(total_mb - free_mb) * 100.0 / (double)total_mb;
                d.free_percent = 100.0 - d.usage_percent;
            }
        }

        if (!primarySet && type == DRIVE_FIXED) {
            dev.primary_disk_type = dt;
            primarySet = true;
        }

        dev.disks.push_back(d);
    }
}

#endif // _WIN32

// ========================== LINUX ==========================

#ifdef __linux__

void HardwareInfoProvider::fillOSInfoLinux(ArgentumDevice& dev) const
{
    dev.platform = "Linux";

    struct utsname u;
    if (uname(&u) == 0) {
        dev.os = u.sysname;
        dev.os_kernel = u.release;
        dev.os_arch = u.machine;
    }
    else {
        dev.os = "Linux (uname failed)";
    }
}

void HardwareInfoProvider::fillCPULinux(ArgentumDevice& dev) const
{
    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    if (nprocs > 0)
        dev.cpu_cores = static_cast<uint32_t>(nprocs);

    FILE* f = std::fopen("/proc/cpuinfo", "r");
    if (!f) return;

    char buf[512];
    while (std::fgets(buf, sizeof(buf), f)) {
        if (std::strncmp(buf, "model name", 10) == 0) {
            char* colon = std::strchr(buf, ':');
            if (colon) {
                std::string s = colon + 1;
                while (!s.empty() && (s.front() == ' ' || s.front() == '\t'))
                    s.erase(s.begin());
                while (!s.empty() && (s.back() == '\n' || s.back() == '\r'))
                    s.pop_back();
                dev.cpu_model = s;
                break;
            }
        }
    }
    std::fclose(f);
}

void HardwareInfoProvider::fillRAMLinux(ArgentumDevice& dev) const
{
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        uint64_t total_mb = info.totalram;
        total_mb = total_mb * info.mem_unit / (1024ull * 1024ull);

        uint64_t free_mb = info.freeram;
        free_mb = free_mb * info.mem_unit / (1024ull * 1024ull);

        dev.ram_mb = total_mb;
        dev.ram_available_mb = free_mb;
        dev.ram_used_mb = total_mb - free_mb;
        dev.ram_usage_percent =
            (double)(total_mb - free_mb) * 100.0 / (double)total_mb;
    }
}

void HardwareInfoProvider::fillGPULinux(ArgentumDevice& dev) const
{
    // Дуже спрощено: читаємо lspci, якщо доступний
    FILE* pipe = popen("lspci | grep -i 'vga\\|3d\\|display'", "r");
    if (!pipe)
        return;

    char line[512];
    while (fgets(line, sizeof(line), pipe)) {
        std::string s(line);
        while (!s.empty() && (s.back() == '\n' || s.back() == '\r'))
            s.pop_back();

        GPUInfo g;
        g.model = s;
        dev.gpus.push_back(g);
    }
    pclose(pipe);
}

void HardwareInfoProvider::fillDisksLinux(ArgentumDevice& dev) const
{
    // Поки що тільки root "/"
    struct statvfs vfs;
    if (statvfs("/", &vfs) == 0) {
        DiskInfo d;
        d.mount_point = "/";
        d.filesystem = "unknown";

        uint64_t total = (uint64_t)vfs.f_blocks * (uint64_t)vfs.f_frsize;
        uint64_t free = (uint64_t)vfs.f_bfree * (uint64_t)vfs.f_frsize;

        d.total_mb = total / (1024ull * 1024ull);
        d.free_mb = free / (1024ull * 1024ull);
        d.used_mb = d.total_mb - d.free_mb;
        if (d.total_mb > 0) {
            d.usage_percent = (double)(d.total_mb - d.free_mb) * 100.0 / (double)d.total_mb;
            d.free_percent = 100.0 - d.usage_percent;
        }

        d.type = DiskType::Unknown;
        dev.primary_disk_type = DiskType::Unknown;

        dev.disks.push_back(d);
    }
}

#endif // __linux__
