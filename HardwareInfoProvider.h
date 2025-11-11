#ifndef HARDWAREINFOPROVIDER_H
#define HARDWAREINFOPROVIDER_H

#include <QString>
#include <QList>

// Структура для інформації про диск
struct DiskInfo
{
    QString mountPoint;      // C:\, D:\, /, /home
    QString fileSystem;      // NTFS, ext4, тощо
    QString type;            // SSD, HDD, Unknown
    QString model;           // Модель диску
    quint64 totalBytes;      // Загальний розмір
    quint64 freeBytes;       // Вільне місце
    quint64 usedBytes;       // Використано
    double usagePercent;     // Відсоток використання
    
    DiskInfo() : totalBytes(0), freeBytes(0), usedBytes(0), usagePercent(0.0) {}
};

class HardwareInfoProvider
{
public:
    HardwareInfoProvider();
    ~HardwareInfoProvider();

    // Інформація про ОС
    QString getOSInfo() const;
    QString getKernelVersion() const;
    QString getArchitecture() const;
    QString getPlatformName() const;

    // Інформація про CPU
    QString getCPUName() const;
    int getCPUCores() const;
    int getCPUFrequencyMHz() const;
    double getCPUFrequencyGHz() const;

    // Інформація про RAM
    quint64 getTotalRAM() const;
    quint64 getAvailableRAM() const;
    quint64 getUsedRAM() const;
    double getRAMUsagePercent() const;

    // Інформація про GPU
    QString getGPUInfo() const;
    QString getGPUName() const;
    quint64 getGPUMemoryMB() const;
    quint64 getGPUUsedMemoryMB() const;
    quint64 getGPUFreeMemoryMB() const;
    double getGPUMemoryUsagePercent() const;

    // Інформація про диски
    QList<DiskInfo> getDisks() const;
    quint64 getTotalDiskSpace() const;
    quint64 getUsedDiskSpace() const;
    quint64 getFreeDiskSpace() const;
    double getDiskUsagePercent() const;

    // Форматування
    QString formatBytes(quint64 bytes) const;

    // Отримання всієї інформації
    QString getAllSystemInfo() const;

private:
    // Платформо-специфічні методи
#ifdef _WIN32
    int getCPUFrequencyFromRegistry() const;
    QString getCPUNameFromRegistry() const;
    QString getWindowsGPUInfo() const;
    QString getWindowsDiskType(const QString &drive) const;
#endif

#ifdef __linux__
    QString getLinuxCPUInfo() const;
    quint64 getLinuxTotalRAM() const;
    quint64 getLinuxAvailableRAM() const;
    QString getLinuxGPUInfo() const;
    QString getLinuxGPUFromSys() const;
    QString getLinuxGPUFromLspci() const;
    QString getLinuxDiskType(const QString &device) const;
#endif
};

#endif // HARDWAREINFOPROVIDER_H
