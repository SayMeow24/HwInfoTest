#ifndef HARDWAREINFOPROVIDER_H
#define HARDWAREINFOPROVIDER_H

#include <string>
#include <vector>
#include <optional>
#include <cstdint>

// ========================== DiskType ==========================

enum class DiskType : uint8_t {
    Unknown = 0,
    HDD     = 1,
    SSD     = 2,
    External = 3,
    Removable = 4
};

// ========================== GPUInfo ==========================

struct GPUInfo {
    std::string model;
    std::optional<uint64_t> vram_mb;
    std::optional<uint64_t> vram_used_mb;
    std::optional<uint64_t> vram_free_mb;
    std::optional<double>   vram_usage_percent;
};

// ========================== DiskInfo ==========================

struct DiskInfo {
    std::string mount_point;
    std::string filesystem;
    DiskType    type = DiskType::Unknown;

    uint64_t total_mb = 0;
    uint64_t free_mb  = 0;
    uint64_t used_mb  = 0;

    double usage_percent = 0.0;
    double free_percent  = 0.0;
};

// ========================== ArgentumDevice ==========================

struct ArgentumDevice {
    // Platform & OS
    std::optional<std::string> platform;
    std::string                os;
    std::optional<std::string> os_kernel;
    std::optional<std::string> os_arch;

    // CPU
    std::optional<std::string> cpu_model;
    uint32_t                   cpu_cores = 0;
    std::optional<uint32_t>    cpu_frequency_mhz;

    // RAM
    uint64_t                   ram_mb = 0;
    std::optional<uint64_t>    ram_available_mb;
    std::optional<uint64_t>    ram_used_mb;
    std::optional<double>      ram_usage_percent;

    // GPU
    std::optional<uint32_t>    gpu_count;
    std::vector<GPUInfo>       gpus;

    // Disks
    std::vector<DiskInfo>      disks;
    std::optional<uint64_t>    total_disk_mb;
    std::optional<uint64_t>    free_disk_mb;
    std::optional<uint64_t>    used_disk_mb;
    std::optional<double>      disk_usage_percent;

    DiskType                   primary_disk_type = DiskType::Unknown;
};

// ========================== HardwareInfoProvider ==========================

class HardwareInfoProvider
{
public:
    HardwareInfoProvider() = default;
    ~HardwareInfoProvider() = default;

    // Головний метод – зібрати всю інформацію
    ArgentumDevice getDeviceInfo() const;

    // Візуальний структурний вивід (для дебагу / CLI)
    static void printStructure(const ArgentumDevice& dev);

    // Вивід у JSON-рядок (для інтеграції)
    static std::string toJSON(const ArgentumDevice& dev);

    // Хелпер
    static std::string diskTypeToString(DiskType type);

private:
#ifdef _WIN32
    void fillOSInfoWindows(ArgentumDevice& dev) const;
    void fillCPUWindows(ArgentumDevice& dev) const;
    void fillRAMWindows(ArgentumDevice& dev) const;
    void fillGPUWindows(ArgentumDevice& dev) const;
    void fillDisksWindows(ArgentumDevice& dev) const;
#endif

#ifdef __linux__
    void fillOSInfoLinux(ArgentumDevice& dev) const;
    void fillCPULinux(ArgentumDevice& dev) const;
    void fillRAMLinux(ArgentumDevice& dev) const;
    void fillGPULinux(ArgentumDevice& dev) const;
    void fillDisksLinux(ArgentumDevice& dev) const;
#endif

    static std::string formatBytesMB(uint64_t mb);
    static std::string escapeJson(const std::string& s);
};

#endif // HARDWAREINFOPROVIDER_H
