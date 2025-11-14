#pragma once
#include <string>
#include <vector>
#include <optional>
#include <cstdint>

// ================================
//   OS INFO
// ================================
struct OSInfo {
    std::string name;
    std::string kernel;
    std::string architecture;
    std::string platform;
};

// ================================
//   CPU INFO
// ================================
struct CPUInfo {
    std::string model;
    uint32_t cores = 0;
    uint64_t frequency_mhz = 0;
};

// ================================
//   RAM INFO
// ================================
struct RAMInfo {
    uint64_t total_mb = 0;
    uint64_t available_mb = 0;

    uint64_t used_mb() const {
        return (total_mb > available_mb ? total_mb - available_mb : 0);
    }
    double usage_percent() const {
        return total_mb > 0 ? (double)used_mb() * 100.0 / (double)total_mb : 0.0;
    }
};

// ================================
//   GPU INFO (оновлено)
// ================================
struct GPUInfo {
    std::string model;

    // Total VRAM (мегабайти)
    uint64_t vram_mb = 0;

    // VRAM usage (AMD/NVIDIA Windows + Linux)
    uint64_t vram_used_mb = 0;
    uint64_t vram_free_mb = 0;
    double   vram_usage_percent = 0.0;
};

// ================================
//   DISK INFO
// ================================
struct DiskInfo {
    std::string mount_point;
    std::string filesystem;
    std::string type;
    uint64_t total_mb = 0;
    uint64_t free_mb = 0;

    uint64_t used_mb() const {
        return total_mb > free_mb ? total_mb - free_mb : 0;
    }
    double usage_percent() const {
        return total_mb > 0 ? (double)used_mb() * 100.0 / (double)total_mb : 0.0;
    }
    double free_percent() const {
        return 100.0 - usage_percent();
    }
};

// ================================
//   FULL DEVICE INFO
// ================================
struct ArgentumDevice {
    OSInfo os;
    CPUInfo cpu;
    RAMInfo ram;

    std::vector<GPUInfo> gpus;
    std::vector<DiskInfo> disks;

    // aggregated disk data
    uint64_t total_disk_mb = 0;
    uint64_t free_disk_mb = 0;
    uint64_t used_disk_mb = 0;
    double disk_usage_percent = 0.0;
};

