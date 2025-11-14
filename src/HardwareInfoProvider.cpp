#include "HardwareInfoProvider.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <sysinfoapi.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#endif

#ifdef __linux__
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#endif

// ========================== Helpers ==========================

std::string HardwareInfoProvider::formatMBToGB(uint64_t mb)
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

// ======================== getDeviceInfo =======================

ArgentumDevice HardwareInfoProvider::getDeviceInfo() const
{
    ArgentumDevice dev;

#ifdef _WIN32
    fillOSWindows(dev);
    fillCPUWindows(dev);
    fillRAMWindows(dev);
    fillGPUWindows(dev);
    fillDisksWindows(dev);
#elif defined(__linux__)
    fillOSLinux(dev);
    fillCPULinux(dev);
    fillRAMLinux(dev);
    fillGPULinux(dev);
    fillDisksLinux(dev);
#else
    dev.os.name = "Unknown OS";
    dev.os.platform = "Unknown";
#endif

    // aggregated disks
    uint64_t total = 0, free = 0;
    for (const auto& d : dev.disks) {
        total += d.total_mb;
        free += d.free_mb;
    }
    dev.total_disk_mb = total;
    dev.free_disk_mb = free;
    dev.used_disk_mb = (total > free ? total - free : 0);
    dev.disk_usage_percent =
        (total > 0 ? static_cast<double>(dev.used_disk_mb) * 100.0 / static_cast<double>(total) : 0.0);

    return dev;
}

// ========================== printDeviceInfo ==========================

void HardwareInfoProvider::printDeviceInfo(const ArgentumDevice& dev)
{
    std::cout << "=== ArgentumDevice Structure ===\n\n";

    std::cout << "Platform: " << dev.os.platform << "\n\n";

    std::cout << "OS:\n";
    std::cout << "  Name: " << dev.os.name << "\n";
    std::cout << "  Kernel: " << dev.os.kernel << "\n";
    std::cout << "  Architecture: " << dev.os.architecture << "\n\n";

    std::cout << "CPU:\n";
    std::cout << "  Model: " << dev.cpu.model << "\n";
    std::cout << "  Cores: " << dev.cpu.cores << "\n";
    std::cout << "  Frequency: " << dev.cpu.frequency_mhz << " MHz\n\n";

    std::cout << "RAM:\n";
    std::cout << "  Total: " << dev.ram.total_mb << " MB ("
        << formatMBToGB(dev.ram.total_mb) << ")\n";
    std::cout << "  Available: " << dev.ram.available_mb << " MB\n";
    std::cout << "  Used: " << dev.ram.used_mb() << " MB\n";
    std::cout << "  Usage: " << std::fixed << std::setprecision(1)
        << dev.ram.usage_percent() << "%\n\n";

    std::cout << "GPU:\n";
    if (!dev.gpus.empty())
        std::cout << "  Count: " << dev.gpus.size() << "\n\n";

    for (size_t i = 0; i < dev.gpus.size(); ++i) {
        const auto& g = dev.gpus[i];
        std::cout << "  GPU " << (i + 1) << ": " << g.model << "\n";
        std::cout << "    VRAM Total: " << g.vram_mb << " MB ("
            << formatMBToGB(g.vram_mb) << ")\n";
        std::cout << "    VRAM Used:  " << g.vram_used_mb << " MB\n";
        std::cout << "    VRAM Free:  " << g.vram_free_mb << " MB\n\n";
    }

    std::cout << "Disks:\n";
    for (const auto& d : dev.disks) {
        std::cout << "  " << d.mount_point << " (" << d.filesystem << ")\n";
        std::cout << "    Type: " << d.type << "\n";
        std::cout << "    Size: " << d.total_mb << " MB ("
            << formatMBToGB(d.total_mb) << ")\n";
        std::cout << "    Free: " << d.free_mb << " MB\n";
        std::cout << "    Used: " << d.used_mb() << " MB\n";
        std::cout << "    Usage: "
            << std::fixed << std::setprecision(1)
            << d.usage_percent() << "%  (Free: "
            << d.free_percent() << "%)\n\n";
    }

    std::cout << "Total Disks:\n";
    std::cout << "  Total: " << dev.total_disk_mb << " MB ("
        << formatMBToGB(dev.total_disk_mb) << ")\n";
    std::cout << "  Free: " << dev.free_disk_mb << " MB\n";
    std::cout << "  Used: " << dev.used_disk_mb << " MB ("
        << std::fixed << std::setprecision(1)
        << dev.disk_usage_percent << "%)\n\n";
}

// ========================== toJSON ==========================

std::string HardwareInfoProvider::toJSON(const ArgentumDevice& dev)
{
    std::ostringstream out;
    auto indent = [](int n) { return std::string(n, ' '); };
    int ind = 2;

    out << "{\n";

    // OS
    out << indent(ind) << "\"os\": \"" << escapeJson(dev.os.name) << "\",\n";
    out << indent(ind) << "\"platform\": \"" << escapeJson(dev.os.platform) << "\",\n";
    out << indent(ind) << "\"os_kernel\": \"" << escapeJson(dev.os.kernel) << "\",\n";
    out << indent(ind) << "\"os_arch\": \"" << escapeJson(dev.os.architecture) << "\",\n";

    // CPU
    out << indent(ind) << "\"cpu_model\": \"" << escapeJson(dev.cpu.model) << "\",\n";
    out << indent(ind) << "\"cpu_cores\": " << dev.cpu.cores << ",\n";
    out << indent(ind) << "\"cpu_frequency_mhz\": " << dev.cpu.frequency_mhz << ",\n";

    // RAM
    out << indent(ind) << "\"ram_mb\": " << dev.ram.total_mb << ",\n";
    out << indent(ind) << "\"ram_available_mb\": " << dev.ram.available_mb << ",\n";
    out << indent(ind) << "\"ram_used_mb\": " << dev.ram.used_mb() << ",\n";
    out << indent(ind) << "\"ram_usage_percent\": "
        << dev.ram.usage_percent() << ",\n";

    // GPUs
    out << indent(ind) << "\"gpus\": [\n";
    for (size_t i = 0; i < dev.gpus.size(); ++i) {
        const auto& g = dev.gpus[i];
        out << indent(ind + 2) << "{\n";
        out << indent(ind + 4) << "\"model\": \"" << escapeJson(g.model) << "\",\n";
        out << indent(ind + 4) << "\"vram_mb\": " << g.vram_mb << ",\n";
        out << indent(ind + 4) << "\"vram_used_mb\": " << g.vram_used_mb << ",\n";
        out << indent(ind + 4) << "\"vram_free_mb\": " << g.vram_free_mb << "\n";
        out << indent(ind + 2) << "}";
        if (i + 1 < dev.gpus.size()) out << ",";
        out << "\n";
    }
    out << indent(ind) << "],\n";
    out << indent(ind) << "\"gpu_count\": " << dev.gpus.size() << ",\n";

    // Disks
    out << indent(ind) << "\"disks\": [\n";
    for (size_t i = 0; i < dev.disks.size(); ++i) {
        const auto& d = dev.disks[i];
        out << indent(ind + 2) << "{\n";
        out << indent(ind + 4) << "\"mount_point\": \"" << escapeJson(d.mount_point) << "\",\n";
        out << indent(ind + 4) << "\"filesystem\": \"" << escapeJson(d.filesystem) << "\",\n";
        out << indent(ind + 4) << "\"type\": \"" << escapeJson(d.type) << "\",\n";
        out << indent(ind + 4) << "\"total_mb\": " << d.total_mb << ",\n";
        out << indent(ind + 4) << "\"free_mb\": " << d.free_mb << ",\n";
        out << indent(ind + 4) << "\"used_mb\": " << d.used_mb() << ",\n";
        out << indent(ind + 4) << "\"usage_percent\": " << d.usage_percent() << ",\n";
        out << indent(ind + 4) << "\"free_percent\": " << d.free_percent() << "\n";
        out << indent(ind + 2) << "}";
        if (i + 1 < dev.disks.size()) out << ",";
        out << "\n";
    }
    out << indent(ind) << "],\n";

    // aggregated disks
    out << indent(ind) << "\"total_disk_mb\": " << dev.total_disk_mb << ",\n";
    out << indent(ind) << "\"free_disk_mb\": " << dev.free_disk_mb << ",\n";
    out << indent(ind) << "\"used_disk_mb\": " << dev.used_disk_mb << ",\n";
    out << indent(ind) << "\"disk_usage_percent\": " << dev.disk_usage_percent << "\n";

    out << "}\n";
    return out.str();
}

// ============================= WINDOWS =============================

#ifdef _WIN32

void HardwareInfoProvider::fillOSWindows(ArgentumDevice& dev) const
{
    dev.os.platform = "Windows";

    OSVERSIONINFOEXA osvi{};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
#pragma warning(push)
#pragma warning(disable: 4996)
    if (GetVersionExA(reinterpret_cast<OSVERSIONINFOA*>(&osvi))) {
#pragma warning(pop)
        std::ostringstream oss;
        oss << "Windows " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion;
        dev.os.name = oss.str();

        std::ostringstream k;
        k << osvi.dwMajorVersion << "."
            << osvi.dwMinorVersion << "."
            << osvi.dwBuildNumber;
        dev.os.kernel = k.str();
    }
    else {
        dev.os.name = "Windows (unknown version)";
        dev.os.kernel = "unknown";
    }

    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    switch (si.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:
        dev.os.architecture = "x86_64"; break;
    case PROCESSOR_ARCHITECTURE_INTEL:
        dev.os.architecture = "x86";    break;
    default:
        dev.os.architecture = "Unknown"; break;
    }
}

void HardwareInfoProvider::fillCPUWindows(ArgentumDevice& dev) const
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    dev.cpu.cores = si.dwNumberOfProcessors;

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        char name[256];
        DWORD size = sizeof(name);
        if (RegQueryValueExA(hKey, "ProcessorNameString", nullptr, nullptr,
            reinterpret_cast<LPBYTE>(name), &size) == ERROR_SUCCESS)
        {
            dev.cpu.model = std::string(name);
        }

        DWORD mhz = 0;
        size = sizeof(mhz);
        if (RegQueryValueExA(hKey, "~MHz", nullptr, nullptr,
            reinterpret_cast<LPBYTE>(&mhz), &size) == ERROR_SUCCESS)
        {
            dev.cpu.frequency_mhz = mhz;
        }
        RegCloseKey(hKey);
    }
}

void HardwareInfoProvider::fillRAMWindows(ArgentumDevice& dev) const
{
    MEMORYSTATUSEX ms{};
    ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms)) {
        uint64_t total_mb = ms.ullTotalPhys / (1024ull * 1024ull);
        uint64_t avail_mb = ms.ullAvailPhys / (1024ull * 1024ull);
        dev.ram.total_mb = total_mb;
        dev.ram.available_mb = avail_mb;
    }
}

void HardwareInfoProvider::fillGPUWindows(ArgentumDevice& dev) const
{
    IDXGIFactory6* factory = nullptr;
    if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory))))
        return;

    UINT index = 0;
    IDXGIAdapter1* adapter = nullptr;

    while (factory->EnumAdapters1(index, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 desc{};
        if (SUCCEEDED(adapter->GetDesc1(&desc)))
        {
            char name[256] = {};
            wcstombs(name, desc.Description, sizeof(name) - 1);
            std::string model(name);

            // skip Microsoft Basic Render Driver
            if (model.find("Microsoft Basic Render Driver") != std::string::npos) {
                adapter->Release();
                ++index;
                continue;
            }

            GPUInfo gpu;
            gpu.model = model;

            uint64_t total_vram_mb =
                desc.DedicatedVideoMemory / (1024ull * 1024ull);
            gpu.vram_mb = total_vram_mb;

            bool success = false;

            IDXGIAdapter3* adapter3 = nullptr;
            if (SUCCEEDED(adapter->QueryInterface(IID_PPV_ARGS(&adapter3))))
            {
                DXGI_QUERY_VIDEO_MEMORY_INFO info{};
                if (SUCCEEDED(adapter3->QueryVideoMemoryInfo(
                    0,
                    DXGI_MEMORY_SEGMENT_GROUP_LOCAL,
                    &info)))
                {
                    uint64_t budget_mb = info.Budget / (1024ull * 1024ull);

                    if (budget_mb > 0)
                    {
                        // Budget = FREE VRAM (особливо на AMD)
                        gpu.vram_free_mb = budget_mb;

                        // Used = Total - Free
                        gpu.vram_used_mb =
                            (total_vram_mb > budget_mb ? total_vram_mb - budget_mb : 0);

                        success = true;
                    }
                }
                adapter3->Release();
            }

            if (!success) {
                gpu.vram_free_mb = 0;
                gpu.vram_used_mb = 0;
            }

            dev.gpus.push_back(gpu);
        }

        adapter->Release();
        ++index;
    }

    factory->Release();
}

void HardwareInfoProvider::fillDisksWindows(ArgentumDevice& dev) const
{
    DWORD mask = GetLogicalDrives();
    if (mask == 0)
        return;

    char root[4] = "A:/";

    for (int i = 0; i < 26; ++i) {
        if (!(mask & (1 << i))) continue;
        root[0] = 'A' + i;

        UINT type = GetDriveTypeA(root);
        if (type == DRIVE_NO_ROOT_DIR) continue;

        DiskInfo d;
        d.mount_point = root;
        d.filesystem = "NTFS"; // спрощено

        switch (type) {
        case DRIVE_FIXED:     d.type = "Fixed";     break;
        case DRIVE_REMOVABLE: d.type = "Removable"; break;
        case DRIVE_CDROM:     d.type = "CDROM";     break;
        case DRIVE_REMOTE:    d.type = "Network";   break;
        default:              d.type = "Unknown";   break;
        }

        ULARGE_INTEGER freeBytesAvailable, totalBytes, freeBytes;
        if (GetDiskFreeSpaceExA(root, &freeBytesAvailable, &totalBytes, &freeBytes)) {
            uint64_t total_mb = totalBytes.QuadPart / (1024ull * 1024ull);
            uint64_t free_mb = freeBytes.QuadPart / (1024ull * 1024ull);
            d.total_mb = total_mb;
            d.free_mb = free_mb;
        }

        dev.disks.push_back(d);
    }
}

#endif // _WIN32

// ============================= LINUX =============================

#ifdef __linux__

static bool readUint64FromFile(const std::string& path, uint64_t& value)
{
    std::ifstream in(path);
    if (!in.is_open())
        return false;

    long long v = 0;
    in >> v;
    if (!in.fail() && v >= 0) {
        value = static_cast<uint64_t>(v);
        return true;
    }
    return false;
}

void HardwareInfoProvider::fillOSLinux(ArgentumDevice& dev) const
{
    dev.os.platform = "Linux";

    struct utsname u {};
    if (uname(&u) == 0) {
        dev.os.name = u.sysname;
        dev.os.kernel = u.release;
        dev.os.architecture = u.machine;
    }
    else {
        dev.os.name = "Linux (uname failed)";
        dev.os.kernel = "unknown";
        dev.os.architecture = "unknown";
    }
}

void HardwareInfoProvider::fillCPULinux(ArgentumDevice& dev) const
{
    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    if (nprocs > 0)
        dev.cpu.cores = static_cast<uint32_t>(nprocs);

    FILE* f = std::fopen("/proc/cpuinfo", "r");
    if (!f) return;

    char buf[512];
    while (std::fgets(buf, sizeof(buf), f)) {
        if (std::strncmp(buf, "model name", 10) == 0) {
            char* colon = std::strchr(buf, ':');
            if (colon) {
                colon++;
                while (*colon == ' ' || *colon == '\t') ++colon;
                std::string name = colon;
                while (!name.empty() && (name.back() == '\n' || name.back() == '\r'))
                    name.pop_back();
                dev.cpu.model = name;
                break;
            }
        }
    }
    std::fclose(f);
}

void HardwareInfoProvider::fillRAMLinux(ArgentumDevice& dev) const
{
    struct sysinfo info {};
    if (sysinfo(&info) == 0) {
        uint64_t total_mb = static_cast<uint64_t>(info.totalram) * info.mem_unit / (1024ull * 1024ull);
        uint64_t free_mb = static_cast<uint64_t>(info.freeram) * info.mem_unit / (1024ull * 1024ull);
        dev.ram.total_mb = total_mb;
        dev.ram.available_mb = free_mb;
    }
}

void HardwareInfoProvider::fillGPULinux(ArgentumDevice& dev) const
{
    namespace fs = std::filesystem;

    std::vector<std::string> gpuModels;

    // 1) lspci для назв
    if (FILE* pipe = popen("lspci | grep -i 'vga\\|3d\\|display'", "r")) {
        char line[512];
        while (fgets(line, sizeof(line), pipe)) {
            std::string s(line);
            while (!s.empty() && (s.back() == '\n' || s.back() == '\r'))
                s.pop_back();
            gpuModels.push_back(s);
        }
        pclose(pipe);
    }

    std::vector<GPUInfo> gpus;

    // 2) AMD (amdgpu): sysfs mem_info_vram_*
    for (int cardIndex = 0; cardIndex < 8; ++cardIndex) {
        std::string base = "/sys/class/drm/card" + std::to_string(cardIndex) + "/device";
        fs::path totalPath = fs::path(base) / "mem_info_vram_total";
        fs::path usedPath = fs::path(base) / "mem_info_vram_used";

        if (fs::exists(totalPath) && fs::exists(usedPath)) {
            uint64_t total_bytes = 0;
            uint64_t used_bytes = 0;

            if (readUint64FromFile(totalPath.string(), total_bytes) &&
                readUint64FromFile(usedPath.string(), used_bytes))
            {
                GPUInfo g;

                if (!gpuModels.empty()) {
                    g.model = gpuModels.front();
                    gpuModels.erase(gpuModels.begin());
                }
                else {
                    g.model = "GPU card" + std::to_string(cardIndex);
                }

                uint64_t total_mb = total_bytes / (1024ull * 1024ull);
                uint64_t used_mb = used_bytes / (1024ull * 1024ull);

                g.vram_mb = total_mb;
                g.vram_used_mb = used_mb;
                g.vram_free_mb = (total_mb > used_mb ? total_mb - used_mb : 0);

                gpus.push_back(g);
            }
        }
    }

    // 3) NVIDIA: nvidia-smi
    if (gpus.empty()) {
        if (FILE* pipe = popen("nvidia-smi --query-gpu=memory.total,memory.used --format=csv,noheader,nounits", "r")) {
            char line[256];
            int idx = 0;
            while (fgets(line, sizeof(line), pipe)) {
                unsigned long long total_mb = 0;
                unsigned long long used_mb = 0;
                if (sscanf(line, "%llu , %llu", &total_mb, &used_mb) == 2 ||
                    sscanf(line, "%llu, %llu", &total_mb, &used_mb) == 2)
                {
                    GPUInfo g;
                    if (idx < static_cast<int>(gpuModels.size()))
                        g.model = gpuModels[idx];
                    else
                        g.model = "NVIDIA GPU " + std::to_string(idx);

                    g.vram_mb = static_cast<uint64_t>(total_mb);
                    g.vram_used_mb = static_cast<uint64_t>(used_mb);
                    g.vram_free_mb =
                        (total_mb > used_mb ? static_cast<uint64_t>(total_mb - used_mb) : 0ull);

                    gpus.push_back(g);
                    ++idx;
                }
            }
            pclose(pipe);
        }
    }

    // 4) fallback: тільки моделі
    if (gpus.empty() && !gpuModels.empty()) {
        for (const auto& s : gpuModels) {
            GPUInfo g;
            g.model = s;
            gpus.push_back(g);
        }
    }

    for (const auto& g : gpus)
        dev.gpus.push_back(g);
}

void HardwareInfoProvider::fillDisksLinux(ArgentumDevice& dev) const
{
    FILE* f = std::fopen("/proc/mounts", "r");
    if (!f) return;

    char devName[256];
    char mountPoint[256];
    char fsType[64];
    char opts[256];
    int freq, passno;

    while (std::fscanf(f, "%255s %255s %63s %255s %d %d\n",
        devName, mountPoint, fsType, opts, &freq, &passno) == 6)
    {
        std::string mp = mountPoint;
        std::string fs = fsType;

        if (mp.rfind("/sys", 0) == 0 || mp.rfind("/proc", 0) == 0 ||
            mp.rfind("/run", 0) == 0 || mp.rfind("/dev", 0) == 0)
            continue;

        struct statvfs vfs {};
        if (statvfs(mountPoint, &vfs) != 0)
            continue;

        uint64_t total_mb = static_cast<uint64_t>(vfs.f_blocks) * vfs.f_frsize / (1024ull * 1024ull);
        uint64_t free_mb = static_cast<uint64_t>(vfs.f_bfree) * vfs.f_frsize / (1024ull * 1024ull);

        DiskInfo d;
        d.mount_point = mp;
        d.filesystem = fs;
        d.type = "LinuxFS";
        d.total_mb = total_mb;
        d.free_mb = free_mb;

        dev.disks.push_back(d);
    }

    std::fclose(f);
}

#endif // __linux__
