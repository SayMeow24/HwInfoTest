#ifndef HARDWAREINFOPROVIDER_H
#define HARDWAREINFOPROVIDER_H

#include <QString>
#include <QList>
#include <optional>
#include <string>
#include <vector>
#include <cstdint>

// ========================================
// Enum для типів дисків
// ========================================
enum class DiskType : uint8_t {
    Unknown = 0,
    HDD = 1,
    SSD = 2,
    External = 3,
    Removable = 4
};

// ========================================
// Структура для одного диску
// ========================================
struct DiskInfo {
    std::string mount_point;     // C:/, E:/, F:/
    std::string filesystem;      // NTFS, ext4
    DiskType type;               // enum: SSD, HDD, External, Removable
    uint64_t total_mb;           // Розмір в MB
    uint64_t free_mb;            // Вільно в MB
    uint64_t used_mb;            // Використано в MB
    double usage_percent;        // Відсоток використання
    double free_percent;         // Відсоток вільного місця
};

// ========================================
// Структура для одного GPU
// ========================================
struct GPUInfo {
    std::string model;                        // AMD Radeon RX 6600 XT
    std::optional<uint64_t> vram_mb;          // Загальна VRAM в MB
    std::optional<uint64_t> vram_used_mb;     // Використана VRAM в MB
    std::optional<uint64_t> vram_free_mb;     // Вільна VRAM в MB
    std::optional<double> vram_usage_percent; // Відсоток використання VRAM
};

// ========================================
// Основна структура для Argentum
// ========================================
struct ArgentumDevice {
    // OS
    std::string os;                           // "Windows 11 Version 24H2"
    std::optional<std::string> os_kernel;     // "10.0.26100"
    std::optional<std::string> os_arch;       // "x86_64"
    std::optional<std::string> platform;      // "Windows" або "Linux"
    
    // CPU
    std::optional<std::string> cpu_model;     // "AMD Ryzen 7 5700X3D 8-Core Processor"
    uint32_t cpu_cores;                       // 16
    std::optional<uint32_t> cpu_frequency_mhz; // 3200
    
    // RAM
    uint64_t ram_mb;                          // 32624 MB - загальна
    std::optional<uint64_t> ram_used_mb;      // 11714 MB - використана
    std::optional<uint64_t> ram_available_mb; // 20910 MB - доступна
    std::optional<double> ram_usage_percent;  // 36.0%
    
    // GPU - ТІЛЬКИ СПИСОК
    std::optional<uint32_t> gpu_count;        // 2 - кількість GPU
    std::vector<GPUInfo> gpus;                // Список ВСІХ GPU з повною інфо
    
    // Диски
    DiskType primary_disk_type;               // Тип основного диску (C:\ або /)
    std::vector<DiskInfo> disks;              // Список ВСІХ дисків
    std::optional<uint64_t> total_disk_mb;    // Загальний розмір всіх дисків
    std::optional<uint64_t> free_disk_mb;     // Вільно на всіх дисках
    std::optional<uint64_t> used_disk_mb;     // Використано на всіх дисків
    std::optional<double> disk_usage_percent; // Відсоток використання дисків
    
    // Конструктор
    ArgentumDevice() 
        : cpu_cores(0), 
          ram_mb(0),
          primary_disk_type(DiskType::Unknown) {}
};

// ========================================
// Qt структури (для сумісності зі старим кодом)
// ========================================
struct DiskInfoQt
{
    QString mountPoint;
    QString fileSystem;
    QString type;            // Для сумісності
    DiskType diskType;       // Enum версія
    QString model;
    quint64 totalBytes;
    quint64 freeBytes;
    quint64 usedBytes;
    double usagePercent;
    
    DiskInfoQt() : diskType(DiskType::Unknown), totalBytes(0), freeBytes(0), usedBytes(0), usagePercent(0.0) {}
};

// ========================================
// Клас HardwareInfoProvider
// ========================================
class HardwareInfoProvider
{
public:
    HardwareInfoProvider();
    ~HardwareInfoProvider();

    // ========================================
    // 🔥 ГОЛОВНИЙ МЕТОД - повертає структуру ArgentumDevice
    // ========================================
    ArgentumDevice getDeviceInfo() const;

    // ========================================
    // 🔥 Вивід ArgentumDevice у консоль (для наглядності)
    // ========================================
    static void printDeviceInfo(const ArgentumDevice& device);

    // ========================================
    // Допоміжні методи для конвертації
    // ========================================
    static DiskType stringToDiskType(const QString &typeStr);
    static QString diskTypeToString(DiskType type);
    static std::string diskTypeToStdString(DiskType type);

    // ========================================
    // Інформація про ОС
    // ========================================
    QString getOSInfo() const;
    QString getKernelVersion() const;
    QString getArchitecture() const;
    QString getPlatformName() const;

    // ========================================
    // Інформація про CPU
    // ========================================
    QString getCPUName() const;
    int getCPUCores() const;
    int getCPUFrequencyMHz() const;
    double getCPUFrequencyGHz() const;

    // ========================================
    // Інформація про RAM
    // ========================================
    quint64 getTotalRAM() const;
    quint64 getAvailableRAM() const;
    quint64 getUsedRAM() const;
    double getRAMUsagePercent() const;

    // ========================================
    // Інформація про GPU
    // ========================================
    QString getGPUInfo() const;
    QString getGPUName() const;
    std::vector<GPUInfo> getGPUList() const;  // 🆕 Список всіх GPU
    quint64 getGPUMemoryMB() const;
    quint64 getGPUUsedMemoryMB() const;
    quint64 getGPUFreeMemoryMB() const;
    double getGPUMemoryUsagePercent() const;

    // ========================================
    // Інформація про диски
    // ========================================
    QList<DiskInfoQt> getDisks() const;
    quint64 getTotalDiskSpace() const;
    quint64 getUsedDiskSpace() const;
    quint64 getFreeDiskSpace() const;
    double getDiskUsagePercent() const;

    // ========================================
    // Форматування
    // ========================================
    QString formatBytes(quint64 bytes) const;
    static std::string formatBytesMB(uint64_t mb);  // Форматування MB в GB

    // ========================================
    // Отримання всієї інформації (для сумісності)
    // ========================================
    QString getAllSystemInfo() const;

private:
#ifdef _WIN32
    int getCPUFrequencyFromRegistry() const;
    QString getCPUNameFromRegistry() const;
    QString getWindowsGPUInfo() const;
    std::vector<GPUInfo> getWindowsGPUList() const;  // 🆕
    QString getWindowsDiskType(const QString &drive) const;
#endif

#ifdef __linux__
    QString getLinuxCPUInfo() const;
    quint64 getLinuxTotalRAM() const;
    quint64 getLinuxAvailableRAM() const;
    QString getLinuxGPUInfo() const;
    std::vector<GPUInfo> getLinuxGPUList() const;  // 🆕
    QString getLinuxGPUFromSys() const;
    QString getLinuxGPUFromLspci() const;
    QString getLinuxDiskType(const QString &device) const;
#endif
};

#endif // HARDWAREINFOPROVIDER_H