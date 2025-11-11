# HardwareInfoProvider - Windows & Linux Edition

Кросплатформна бібліотека для отримання інформації про апаратне забезпечення на **Windows** та **Linux**.

## ✅ Підтримка платформ

| Функція | Windows | Linux |
|---------|---------|-------|
| **ОС** | ✅ | ✅ |
| **CPU** (назва, ядра, частота) | ✅ | ✅ |
| **RAM** (загальна, доступна, використана) | ✅ | ✅ |
| **GPU** (назва) | ✅ DirectX | ✅ lspci |
| **VRAM** (загальна) | ✅ | ⚠️ |
| **VRAM** (використана, вільна) | ✅ DXGI 1.4 | ✅ nvidia-smi / sysfs |
| **Диски** (список, розмір, вільне місце) | ✅ | ✅ |
| **Тип диску** (SSD/HDD) | ✅ WMI | ✅ sysfs |

## 🚀 Швидкий старт

### Windows
```bash
# Відкрийте .pro файл у Qt Creator
# Натисніть Run
```

### Linux
```bash
# Встановіть залежності
sudo apt install qt6-base-dev pciutils

# Для NVIDIA GPU (опціонально)
sudo apt install nvidia-utils

# Збірка
qmake6 hwinfo.pro
make
./hwinfo
```

## 📦 Структура файлів

```
├── HardwareInfoProvider.h      # Заголовочний файл
├── HardwareInfoProvider.cpp    # Реалізація
├── main.cpp                    # Приклад
├── CMakeLists.txt              # CMake конфігурація
└── hwinfo.pro                  # qmake конфігурація
```

## 💻 Приклад використання

```cpp
#include <QCoreApplication>
#include <QDebug>
#include "HardwareInfoProvider.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    HardwareInfoProvider hw;
    
    // Вся інформація одразу
    qDebug().noquote() << hw.getAllSystemInfo();
    
    // Або окремо
    qDebug() << "CPU:" << hw.getCPUName();
    qDebug() << "Cores:" << hw.getCPUCores();
    qDebug() << "RAM:" << hw.formatBytes(hw.getTotalRAM());
    qDebug() << "GPU:" << hw.getGPUName();
    qDebug() << "VRAM:" << hw.getGPUMemoryMB() << "MB";
    qDebug() << "VRAM Used:" << hw.getGPUUsedMemoryMB() << "MB";
    
    // Інформація про диски
    QList<DiskInfo> disks = hw.getDisks();
    for (const DiskInfo &disk : disks) {
        qDebug() << "Disk:" << disk.mountPoint;
        qDebug() << "Type:" << disk.type;
        qDebug() << "Size:" << hw.formatBytes(disk.totalBytes);
        qDebug() << "Free:" << hw.formatBytes(disk.freeBytes);
    }
    
    return 0;
}
```

## 📊 Приклад виводу

### Windows
```
=== Системна інформація ===

Платформа: Windows

Операційна система:
  Назва: Windows 11 Pro
  Ядро: 10.0.22621
  Архітектура: x86_64

Процесор:
  Модель: Intel(R) Core(TM) i7-10700K CPU @ 3.80GHz
  Ядра: 16
  Частота: 3800 MHz (3.80 GHz)

Оперативна пам'ять:
  Загальна: 32.00 GB
  Доступна: 18.45 GB
  Використано: 13.55 GB
  Використання: 42.3%

Відеокарта:
  Назва: NVIDIA GeForce RTX 3080
  VRAM Загальна: 10240 MB (10.00 GB)
  VRAM Використано: 3584 MB (3.50 GB)
  VRAM Вільно: 6656 MB (6.50 GB)
  VRAM Використання: 35.0%

Диски:
  C:\ (NTFS)
    Тип: SSD
    Розмір: 500.00 GB
    Вільно: 125.50 GB (25.1%)
    Використано: 374.50 GB (74.9%)
  D:\ (NTFS)
    Тип: HDD
    Розмір: 2.00 TB
    Вільно: 1.20 TB (60.0%)
    Використано: 819.20 GB (40.0%)

  Всього на дисках:
    Загальний розмір: 2.49 TB
    Вільно: 1.31 TB
    Використано: 1.18 TB (47.5%)

===========================
```

### Linux
```
=== Системна інформація ===

Платформа: Linux

Операційна система:
  Назва: Ubuntu 22.04 LTS
  Ядро: 5.15.0-91-generic
  Архітектура: x86_64

Процесор:
  Модель: AMD Ryzen 7 5800X 8-Core Processor
  Ядра: 16
  Частота: 3800 MHz (3.80 GHz)

Оперативна пам'ять:
  Загальна: 32.00 GB
  Доступна: 21.34 GB
  Використано: 10.66 GB
  Використання: 33.3%

Відеокарта:
  Назва: NVIDIA GeForce RTX 3070
  VRAM Загальна: 8192 MB (8.00 GB)
  VRAM Використано: 2458 MB (2.40 GB)
  VRAM Вільно: 5734 MB (5.60 GB)
  VRAM Використання: 30.0%

Диски:
  / (ext4)
    Тип: SSD
    Розмір: 250.00 GB
    Вільно: 89.50 GB (35.8%)
    Використано: 160.50 GB (64.2%)
  /home (ext4)
    Тип: HDD
    Розмір: 1.00 TB
    Вільно: 456.30 GB (44.6%)
    Використано: 567.70 GB (55.4%)

  Всього на дисках:
    Загальний розмір: 1.24 TB
    Вільно: 545.80 GB
    Використано: 728.20 GB (58.4%)

===========================
```

## 🔧 API Методи

### Інформація про ОС
- `QString getOSInfo()` - назва та версія ОС
- `QString getKernelVersion()` - версія ядра
- `QString getArchitecture()` - архітектура (x86_64, тощо)
- `QString getPlatformName()` - "Windows" або "Linux"

### Інформація про CPU
- `QString getCPUName()` - модель процесора
- `int getCPUCores()` - кількість ядер (логічних)
- `int getCPUFrequencyMHz()` - частота в MHz
- `double getCPUFrequencyGHz()` - частота в GHz

### Інформація про RAM
- `quint64 getTotalRAM()` - загальна пам'ять в байтах
- `quint64 getAvailableRAM()` - доступна пам'ять в байтах
- `quint64 getUsedRAM()` - використана пам'ять в байтах
- `double getRAMUsagePercent()` - відсоток використання

### Інформація про GPU
- `QString getGPUInfo()` - повна інформація
- `QString getGPUName()` - назва відеокарти
- `quint64 getGPUMemoryMB()` - загальна VRAM в MB
- `quint64 getGPUUsedMemoryMB()` - використана VRAM в MB
- `quint64 getGPUFreeMemoryMB()` - вільна VRAM в MB
- `double getGPUMemoryUsagePercent()` - відсоток використання VRAM

### Інформація про диски
- `QList<DiskInfo> getDisks()` - список всіх дисків
- `quint64 getTotalDiskSpace()` - загальний розмір всіх дисків (bytes)
- `quint64 getUsedDiskSpace()` - використано на всіх дисках (bytes)
- `quint64 getFreeDiskSpace()` - вільно на всіх дисках (bytes)
- `double getDiskUsagePercent()` - відсоток використання дисків

**Структура DiskInfo:**
- `QString mountPoint` - точка монтування (C:\, D:\, /, /home)
- `QString fileSystem` - файлова система (NTFS, ext4)
- `QString type` - тип диску (SSD, HDD, Unknown)
- `quint64 totalBytes` - загальний розмір
- `quint64 freeBytes` - вільне місце
- `quint64 usedBytes` - використано
- `double usagePercent` - відсоток використання

### Утиліти
- `QString formatBytes(quint64 bytes)` - форматування байтів (B, KB, MB, GB)
- `QString getAllSystemInfo()` - вся інформація у форматованому вигляді

## ⚙️ Вимоги

### Windows
- Qt 6.2+
- MSVC 2019+ або MinGW
- Windows SDK (для DirectX)

### Linux
- Qt 6.2+
- GCC 9+ або Clang 10+
- `pciutils` для GPU: `sudo apt install pciutils`
- `nvidia-utils` для VRAM (NVIDIA): `sudo apt install nvidia-utils`

## 🔨 Збірка

### CMake
```bash
mkdir build && cd build
cmake ..
cmake --build .
./hwinfo
```

### qmake
```bash
qmake hwinfo.pro
make
./hwinfo
```

## 📝 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(hwinfo)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_executable(hwinfo
    main.cpp
    HardwareInfoProvider.cpp
)

target_link_libraries(hwinfo Qt6::Core)

if(WIN32)
    target_link_libraries(hwinfo dxgi wbemuuid)
endif()
```

## 📝 hwinfo.pro

```qmake
QT += core
CONFIG += c++17
TEMPLATE = app

SOURCES += \
    main.cpp \
    HardwareInfoProvider.cpp

HEADERS += \
    HardwareInfoProvider.h

win32: LIBS += -ldxgi -lwbemuuid
```

## 🎯 Платформо-специфічні особливості

### Windows
- **CPU**: Читає з реєстру Windows
- **GPU**: Використовує DirectX (DXGI)
- **VRAM Used**: DXGI 1.4 (Windows 10+)

### Linux
- **CPU**: Читає `/proc/cpuinfo` та `/sys/devices/system/cpu/`
- **RAM**: Використовує `sysinfo()`
- **GPU**: 
  - Назва через `lspci -v`
  - VRAM через `nvidia-smi` (NVIDIA)
  - VRAM через `/sys/class/drm/` (AMD)

## ⚠️ Важливі примітки

### Windows
- Для VRAM Usage потрібен Windows 10+
- Старіші версії Windows повернуть 0 для `getGPUUsedMemoryMB()`

### Linux
- **NVIDIA GPU**: Встановіть `nvidia-utils` для VRAM usage
- **AMD GPU**: Використовується sysfs (драйвери Mesa 20.0+)
- **Intel GPU**: VRAM usage недоступний (інтегровані GPU)

## 🐛 Troubleshooting

### Linux: "GPU information not available"
```bash
# Встановіть lspci
sudo apt install pciutils
```

### Linux NVIDIA: Used VRAM завжди 0
```bash
# Встановіть nvidia-smi
sudo apt install nvidia-utils

# Перевірка
nvidia-smi
```

### Linux AMD: Used VRAM завжди 0
```bash
# Перевірте наявність файлу
ls /sys/class/drm/card*/device/mem_info_vram_used

# Оновіть драйвери якщо немає
sudo apt update && sudo apt upgrade
```

## 📚 Код досить чистий

- ✅ 60 рядків заголовочного файлу
- ✅ 600 рядків реалізації
- ✅ Без залежностей окрім Qt Core
- ✅ Просте додавання у проект
- ✅ Зрозумілий API

## 📄 Ліцензія

Вільне використання у ваших проектах.

---

**Версія:** 2.0 (Windows & Linux Edition)  
**Дата:** Листопад 2025
