#include <QCoreApplication>
#include <QDebug>
#include "HardwareInfoProvider.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    HardwareInfoProvider hw;
    
    // Вся інформація одразу
    qDebug().noquote() << "\n" << hw.getAllSystemInfo() << "\n";
    
    // Додаткова детальна інформація
    qDebug() << "=== Детальна інформація ===";
    qDebug() << "Platform:" << hw.getPlatformName();
    qDebug() << "";
    
    qDebug() << "OS:" << hw.getOSInfo();
    qDebug() << "Kernel:" << hw.getKernelVersion();
    qDebug() << "Arch:" << hw.getArchitecture();
    qDebug() << "";
    
    qDebug() << "CPU:" << hw.getCPUName();
    qDebug() << "CPU Cores:" << hw.getCPUCores();
    qDebug() << "CPU Frequency:" << hw.getCPUFrequencyGHz() << "GHz";
    qDebug() << "";
    
    qDebug() << "Total RAM:" << hw.formatBytes(hw.getTotalRAM());
    qDebug() << "Available RAM:" << hw.formatBytes(hw.getAvailableRAM());
    qDebug() << "Used RAM:" << hw.formatBytes(hw.getUsedRAM());
    qDebug() << "RAM Usage:" << QString::number(hw.getRAMUsagePercent(), 'f', 1) << "%";
    qDebug() << "";
    
    qDebug() << "GPU:" << hw.getGPUName();
    quint64 totalVRAM = hw.getGPUMemoryMB();
    quint64 usedVRAM = hw.getGPUUsedMemoryMB();
    quint64 freeVRAM = hw.getGPUFreeMemoryMB();
    if (totalVRAM > 0) {
        qDebug() << "GPU Total VRAM:" << totalVRAM << "MB";
        if (usedVRAM > 0) {
            qDebug() << "GPU Used VRAM:" << usedVRAM << "MB";
        }
        if (freeVRAM > 0) {
            qDebug() << "GPU Free VRAM:" << freeVRAM << "MB";
        }
        if (usedVRAM > 0) {
            qDebug() << "GPU VRAM Usage:" << QString::number(hw.getGPUMemoryUsagePercent(), 'f', 1) << "%";
        }
    }
    qDebug() << "";
    
    qDebug() << "Total Disk Space:" << hw.formatBytes(hw.getTotalDiskSpace());
    qDebug() << "Free Disk Space:" << hw.formatBytes(hw.getFreeDiskSpace());
    qDebug() << "Disk Usage:" << QString::number(hw.getDiskUsagePercent(), 'f', 1) << "%";
    qDebug() << "";
    
    qDebug() << "Disks:";
    QList<DiskInfo> disks = hw.getDisks();
    for (const DiskInfo &disk : disks) {
        qDebug() << "  -" << disk.mountPoint << "(" << disk.fileSystem << ")";
        qDebug() << "    Type:" << disk.type;
        qDebug() << "    Size:" << hw.formatBytes(disk.totalBytes);
        qDebug() << "    Free:" << hw.formatBytes(disk.freeBytes);
        qDebug() << "    Usage:" << QString::number(disk.usagePercent, 'f', 1) << "%";
    }
    
    return 0;
}
