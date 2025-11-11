# 🧩 HardwareInfoProvider v2.2
Бібліотека для збору системної інформації (CPU, GPU, RAM, Disk, OS)  
Підтримка **Windows** і **Linux**, інтеграція з **Qt / C++**.

---

## ⚙️ Підтримувані платформи

| Компонент | Windows | Linux |
|------------|----------|--------|
| **CPU** (назва, ядра, частота) | ✅ WMI / Win32_Processor | ✅ `/proc/cpuinfo` |
| **GPU** (назва) | ✅ DXGI (через EnumAdapters із фільтрацією дублікатів) | ✅ `lspci` / `/sys/class/drm` |
| **GPU VRAM** (загальна, вільна, використана) | ✅ DXGI 1.4 (Windows 10+) | ✅ `nvidia-smi` / sysfs |
| **RAM** | ✅ GlobalMemoryStatusEx | ✅ `/proc/meminfo` |
| **Disk** (тип, розмір, файлові системи) | ✅ WMI / MSFT_PhysicalDisk | ✅ `lsblk` |
| **OS** (назва, версія, ядро, архітектура) | ✅ WMI / Win32_OperatingSystem | ✅ `/etc/os-release` |

---

## 💻 Загальні методи API

| Метод | Повертає |
|--------|-----------|
| `QString getCPUInfo()` | Назва CPU, кількість ядер, частоту |
| `QString getGPUInfo()` | Повна інформація про всі GPU |
| `QString getGPUName()` | Назва першого GPU |
| `quint64 getGPUMemoryMB()` | Загальний обсяг VRAM |
| `quint64 getGPUFreeMemoryMB()` | Вільна VRAM |
| `quint64 getGPUUsedMemoryMB()` | Використана VRAM |
| `double getGPUMemoryUsagePercent()` | Відсоток використання VRAM |
| `QList<DiskInfo> getDisks()` | Список дисків із типом (SSD/HDD/External/Removable) |
| `QString getOSInfo()` | Назва ОС, версія, архітектура |
| `QString getRAMInfo()` | Загальна, вільна та використана пам’ять |

---

## 🎯 Платформо-специфічні особливості

### 🪟 Windows

- **GPU:**  
  Використовується **DXGI (DirectX Graphics Infrastructure)**, через `EnumAdapters()`  
  із **унікалізацією по VendorId, DeviceId, Name**.  
  Це усуває дублікати адаптерів, які виникали при мульти-GPU конфігураціях (наприклад, iGPU + dGPU).

- **GPU пам’ять:**  
  Через **IDXGIAdapter3::QueryVideoMemoryInfo()** (Windows 10+).  
  Підтримуються поля:
  - `DedicatedVideoMemory`
  - `Budget`
  - `CurrentUsage`

- **Disk Info:**  
  Комбінований метод через **MSFT_PhysicalDisk** і **Win32_DiskDrive**.  
  Підтримує визначення типу:
  - `SSD`, `HDD`, `External`, `Removable`

---

### 🐧 Linux

- **GPU:**  
  Використовується `lspci -v` для аналізу `VGA controller` та `/sys/class/drm`  
  (пошук `vendor` і `device` файлів).  
  Для AMD та NVIDIA додатково визначається VRAM.

- **Disk Info:**  
  Використовується `lsblk -d -o NAME,ROTA,TRAN,TYPE,MODEL`.  
  Підтримується кешування результатів через `QMap` (зменшує кількість викликів процесу).

---

## 🧮 Приклад виводу (Windows)

```
=== Детальна інформація ===
Platform: "Windows"

OS: "Windows 11 Version 24H2"
Kernel: "10.0.26100"
Arch: "x86_64"

CPU: "AMD Ryzen 7 5700X3D 8-Core Processor"
CPU Cores: 16
CPU Frequency: 3.2 GHz

GPU:
  GPU 1: "NVIDIA GeForce RTX 3080"
    Total VRAM: 10240 MB ("10.00 GB")
    Used VRAM: 3584 MB ("3.50 GB")
    Free VRAM: 6656 MB ("6.50 GB")
    Usage: 35.0%

  GPU 2: "Intel(R) UHD Graphics 770"
    Total VRAM: 512 MB ("0.50 GB")
    Used VRAM: 128 MB ("0.12 GB")
    Free VRAM: 384 MB ("0.38 GB")
    Usage: 25.0%

Disk Info:
  - "C:/" (NTFS)
    Type: "SSD"
    Size: "500 GB"
    Free: "320 GB"
    Usage: "36%"
```

---

## 🧠 Додаткові деталі

- **GPU дублікатів більше не буде:**  
  DXGI більше не повертає повтори, бо адаптери фільтруються по `VendorId + DeviceId + Name`.

- **Linux кешує результати `lsblk`:**  
  При повторному виклику функції тип диску (SSD/HDD/External) береться з кешу.

- **Cross-platform узгодження:**  
  `getGPUInfo()` повертає завжди `QString` (об’єднаний список GPU для Windows / одне значення для Linux).

---

## 🧱 Integration Example (Qt)

```cpp
// mainwindow.cpp
#include "HardwareInfoProvider.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    HardwareInfoProvider hw;

    qDebug() << "=== System Info ===";
    qDebug() << "OS:" << hw.getOSInfo();
    qDebug() << "CPU:" << hw.getCPUName();
    qDebug() << "Cores:" << hw.getCpuCoreCount();
    qDebug() << "Frequency:" << hw.getCpuFrequencyGHz() << "GHz";
    qDebug() << "RAM total:" << hw.getTotalRAMGB() << "GB";

#ifdef _WIN32
    qDebug() << "GPU info:" << hw.getGPUInfo();
    qDebug() << "VRAM total:" << hw.getGPUMemoryMB() << "MB";
    qDebug() << "VRAM used:" << hw.getGPUUsedMemoryMB() << "MB";
    qDebug() << "VRAM free:" << hw.getGPUFreeMemoryMB() << "MB";
#else
    qDebug() << "GPU:" << hw.getGPUName();
#endif

    QList<DiskInfo> disks = hw.getDisks();
    for (const auto &disk : disks) {
        qDebug().noquote() << QString("Disk %1 (%2) - %3")
            .arg(disk.mountPoint)
            .arg(disk.fileSystem)
            .arg(disk.type);
    }
}
```

---

### 🧩 Поради:
- Для **Windows** переконайся, що проєкт лінкує `dxgi.lib` та `wbemuuid.lib`  
  (це потрібно для WMI і DXGI).
- Для **Linux** бажано, щоб були доступні утиліти `lsblk` і `lspci` (вони встановлені за замовчуванням у більшості дистрибутивів).
- У **Qt .pro файлі** не забудь додати:
  ```
  QT += core gui
  CONFIG += c++17
  ```

---

## 🏷️ Версія

- **v2.2** — додано Qt integration example  
- **v2.1** — DXGI GPU deduplication + Linux disk caching  
- **v2.0** — Початкова реалізація WMI + `/proc`  
- **v1.0** — Базова версія без VRAM / дисків  
