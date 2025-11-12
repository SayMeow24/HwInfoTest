# 🧩 HardwareInfoProvider  
### Cross-platform system information library for **C++ / Qt**  
Detects CPU, GPU, RAM, Disks, and OS data on **Windows** and **Linux**.

![C++](https://img.shields.io/badge/C%2B%2B-17-blue) ![Qt](https://img.shields.io/badge/Qt-6-green) ![OS](https://img.shields.io/badge/Platforms-Windows%20%7C%20Linux-lightgrey)  
📦 *Version 4.2 — GPU deduplication, LSBLK caching & Qt integration example + Ubuntu disc type fix.*

---

## 🚀 Features
- 🧠 CPU info (name, cores, frequency)
- 🎮 GPU info (multi-GPU with VRAM usage)
- 💾 Disk info (type detection: SSD/HDD/External)
- 🧩 OS & RAM stats
- ⚙️ Qt-friendly API (`QString`, `QList`, etc.)
- 🪟 Uses **DXGI** / **WMI** on Windows
- 🐧 Uses **lsblk**, **lspci**, `/sys` on Linux

---

## 🧱 Quick Start (Qt Example)

```cpp
#include "HardwareInfoProvider.h"
#include <QDebug>

int main() {
    HardwareInfoProvider hw;

    qDebug() << "OS:" << hw.getOSInfo();
    qDebug() << "CPU:" << hw.getCPUName();
    qDebug() << "GPU:" << hw.getGPUInfo();
    qDebug() << "Total VRAM:" << hw.getGPUMemoryMB() << "MB";
    qDebug() << "Total Disks:" << hw.getDisks().size();

    return 0;
}
```

---

## 🧩 Build Requirements
| Platform | Dependencies |
|-----------|---------------|
| **Windows** | `dxgi.lib`, `wbemuuid.lib` |
| **Linux** | `lsblk`, `lspci`, `/sys/class/drm` |

In your **.pro file**:
```pro
QT += core gui
CONFIG += c++17
```

---

## 📄 Full Documentation  
📘 [HardwareInfoProvider_Documentation_EN_v2.2.md](./HardwareInfoProvider_Documentation_EN_v2.2.md)  
📗 [HardwareInfoProvider_Documentation_v2.2.md (Ukrainian)](./HardwareInfoProvider_Documentation_v2.2.md)

---

## 🏷️ Version History
| Version | Changes |
|----------|----------|
| **v2.2** | Added Qt integration example |
| **v2.1** | DXGI deduplication & Linux disk caching |
| **v2.0** | WMI + `/proc` implementation |
| **v1.0** | Base version (no VRAM/disk support) |
