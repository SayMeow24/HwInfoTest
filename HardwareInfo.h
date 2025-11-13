#ifndef HARDWAREINFOPROVIDER_H
#define HARDWAREINFOPROVIDER_H

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <iostream>

// ======================= DiskType =========================

enum class DiskType : uint8_t {
    Unknown = 0,
    HDD     = 1,
    SSD     = 2,
    External = 3,
    Removable = 4
};

struct DiskInfo {
    std::string mount_point;     // "C:/", "/"
    std::string filesystem;      // "NTFS", "ext4"
    DiskType    type;            // SSD / HDD / ...
    uint64_t    total_mb;        // MB
    uint64_t    free_mb;         // MB
    uint64_t    used_mb;         // MB
    double      usage_percent;   // 0–100
    double      free_percent;    // 0–100

    DiskInfo()
        : type(DiskType::Unknown),
          total_mb(0),
          free_mb(0),
          used_mb(0),
          usage_percent(0.0),
          free_percent(0.0)
    {}
};

struct GPUInfo {
    std::string model;                        // "AMD Radeon RX 6600 XT"
    std::optional<uint64_t> vram_mb;          // MB
    std::optional<uint64_t> vram_used_mb;     // MB
    std::optional<uint64_t> vram_free_mb;     // MB
    std::optional<double>   vram_usage_percent;
};

struct ArgentumDevice {
    // OS
    std::string os;                           // "Windows 11 Version 24H2"
    std::optional<std::string> os_kernel;     // "10.0.26100"
    std::optional<std::string> os_arch;       // "x86_64"
    std::optional<std::string> platform;      // "Windows" / "Linux"

    // CPU
    std::optional<std::string> cpu_model;
    uint32_t cpu_cores = 0;
    std::optional<uint32_t> cpu_frequency_mhz;

    // RAM
    uint64_t ram_mb = 0;
    std::optional<uint64_t> ram_used_mb;
    std::optional<uint64_t> ram_available_mb;
    std::optional<double>   ram_usage_percent;

    // GPUs
    std::optional<uint32_t> gpu_count;
    std::vector<GPUInfo>    gpus;

    // Disks
    DiskType primary_disk_type = DiskType::Unknown;
    std::vector<DiskInfo> disks;
    std::optional<uint64_t> total_disk_mb;
    std::optional<uint64_t> free_disk_mb;
    std::optional<uint64_t> used_disk_mb;
    std::optional<double>   disk_usage_percent;
};

// =================== HardwareInfoProvider ====================

class HardwareInfoProvider
{
public:
    HardwareInfoProvider() = default;
    ~HardwareInfoProvider() = default;

    // ГОЛОВНИЙ метод – повертає структуру ArgentumDevice
    ArgentumDevice getDeviceInfo() const;

    // Вивід у консоль (для дебагу / CLI)
    static void printDeviceInfo(const ArgentumDevice& device);

    // Хелпери
    static std::string diskTypeToString(DiskType type);

private:
    // Платформо-залежні частини
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
};

#endif // HARDWAREINFOPROVIDER_H
