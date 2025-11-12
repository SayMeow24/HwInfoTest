#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <iomanip>
#include "HardwareInfoProvider.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    std::cout << "\n";
    std::cout << "=====================================" << std::endl;
    std::cout << "  Hardware Info Provider v4.2" << std::endl;
    std::cout << "  ArgentumDevice Structure Demo" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "\n";

    // Створюємо провайдер
    HardwareInfoProvider hw;

    // Отримуємо структуру ArgentumDevice
    std::cout << "Collecting device information..." << std::endl;
    ArgentumDevice device = hw.getDeviceInfo();
    std::cout << "Done!" << std::endl;
    std::cout << "\n";

    // Виводимо структуру в консоль
    HardwareInfoProvider::printDeviceInfo(device);

    std::cout << "\n";
    std::cout << "=====================================" << std::endl;
    std::cout << "  Accessing Structure Fields Demo" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "\n";

    // Приклад доступу до полів структури
    std::cout << "--- Quick Access Example ---" << std::endl;
    std::cout << "\n";

    // OS
    std::cout << "OS: " << device.os << std::endl;

    // CPU
    if (device.cpu_model.has_value()) {
        std::cout << "CPU: " << device.cpu_model.value()
            << " (" << device.cpu_cores << " cores)" << std::endl;
    }

    // RAM
    std::cout << "RAM: " << HardwareInfoProvider::formatBytesMB(device.ram_mb);
    if (device.ram_usage_percent.has_value()) {
        std::cout << " (" << std::fixed << std::setprecision(1)
            << device.ram_usage_percent.value() << "% used)";
    }
    std::cout << std::endl;

    // GPU Count
    if (device.gpu_count.has_value() && device.gpu_count.value() > 0) {
        std::cout << "GPU Count: " << device.gpu_count.value() << std::endl;

        // Перший GPU
        if (!device.gpus.empty()) {
            const GPUInfo& firstGPU = device.gpus[0];
            std::cout << "Primary GPU: " << firstGPU.model;
            if (firstGPU.vram_mb.has_value()) {
                std::cout << " (" << HardwareInfoProvider::formatBytesMB(firstGPU.vram_mb.value()) << " VRAM)";
            }
            std::cout << std::endl;
        }
    }

    // Disks
    std::cout << "Disks: " << device.disks.size() << " disk(s)" << std::endl;
    std::cout << "Primary Disk Type: "
        << HardwareInfoProvider::diskTypeToStdString(device.primary_disk_type)
        << std::endl;

    std::cout << "\n";

    // Приклад ітерації по GPU
    if (!device.gpus.empty()) {
        std::cout << "--- GPU Details ---" << std::endl;
        std::cout << "\n";

        for (size_t i = 0; i < device.gpus.size(); i++) {
            const GPUInfo& gpu = device.gpus[i];

            std::cout << "GPU " << (i + 1) << ":" << std::endl;
            std::cout << "  Model: " << gpu.model << std::endl;

            if (gpu.vram_mb.has_value()) {
                std::cout << "  VRAM Total: " << gpu.vram_mb.value() << " MB" << std::endl;
            }

            if (gpu.vram_used_mb.has_value()) {
                std::cout << "  VRAM Used: " << gpu.vram_used_mb.value() << " MB" << std::endl;
            }

            if (gpu.vram_free_mb.has_value()) {
                std::cout << "  VRAM Free: " << gpu.vram_free_mb.value() << " MB" << std::endl;
            }

            if (gpu.vram_usage_percent.has_value()) {
                std::cout << "  Usage: " << std::fixed << std::setprecision(1)
                    << gpu.vram_usage_percent.value() << "%" << std::endl;
            }

            std::cout << std::endl;
        }
    }

    // Приклад ітерації по дискам
    if (!device.disks.empty()) {
        std::cout << "--- Disk Details ---" << std::endl;
        std::cout << "\n";

        for (const DiskInfo& disk : device.disks) {
            std::cout << disk.mount_point << " (" << disk.filesystem << "):" << std::endl;
            std::cout << "  Type: " << HardwareInfoProvider::diskTypeToStdString(disk.type) << std::endl;
            std::cout << "  Size: " << HardwareInfoProvider::formatBytesMB(disk.total_mb) << std::endl;
            std::cout << "  Free: " << HardwareInfoProvider::formatBytesMB(disk.free_mb)
                << " (" << std::fixed << std::setprecision(1) << disk.free_percent << "%)" << std::endl;
            std::cout << "  Used: " << HardwareInfoProvider::formatBytesMB(disk.used_mb)
                << " (" << std::fixed << std::setprecision(1) << disk.usage_percent << "%)" << std::endl;
            std::cout << std::endl;
        }
    }

    // JSON-подібний вивід
    std::cout << "=====================================" << std::endl;
    std::cout << "  JSON-like Structure Example" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "\n";

    std::cout << "{" << std::endl;
    std::cout << "  \"os\": \"" << device.os << "\"," << std::endl;
    std::cout << "  \"cpu_cores\": " << device.cpu_cores << "," << std::endl;
    std::cout << "  \"ram_mb\": " << device.ram_mb << "," << std::endl;

    if (device.gpu_count.has_value()) {
        std::cout << "  \"gpu_count\": " << device.gpu_count.value() << "," << std::endl;
    }

    std::cout << "  \"gpus\": [" << std::endl;
    for (size_t i = 0; i < device.gpus.size(); i++) {
        const GPUInfo& gpu = device.gpus[i];
        std::cout << "    {" << std::endl;
        std::cout << "      \"model\": \"" << gpu.model << "\"";

        if (gpu.vram_mb.has_value()) {
            std::cout << "," << std::endl;
            std::cout << "      \"vram_mb\": " << gpu.vram_mb.value();
        }

        std::cout << std::endl;
        std::cout << "    }";
        if (i < device.gpus.size() - 1) {
            std::cout << ",";
        }
        std::cout << std::endl;
    }
    std::cout << "  ]," << std::endl;

    std::cout << "  \"disks\": [" << std::endl;
    for (size_t i = 0; i < device.disks.size(); i++) {
        const DiskInfo& disk = device.disks[i];
        std::cout << "    {" << std::endl;
        std::cout << "      \"mount_point\": \"" << disk.mount_point << "\"," << std::endl;
        std::cout << "      \"type\": \"" << HardwareInfoProvider::diskTypeToStdString(disk.type) << "\"," << std::endl;
        std::cout << "      \"total_mb\": " << disk.total_mb << "," << std::endl;
        std::cout << "      \"free_mb\": " << disk.free_mb << std::endl;
        std::cout << "    }";
        if (i < device.disks.size() - 1) {
            std::cout << ",";
        }
        std::cout << std::endl;
    }
    std::cout << "  ]" << std::endl;
    std::cout << "}" << std::endl;

    std::cout << "\n";
    std::cout << "=====================================" << std::endl;
    std::cout << "  Program finished successfully!" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "\n";

    return 0;
}