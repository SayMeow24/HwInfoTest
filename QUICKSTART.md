# 🚀 Quick Start

## Windows

### 1. Відкрийте Qt Creator
```
File → Open File or Project → hwinfo.pro
```

### 2. Виберіть Kit
```
Desktop Qt 6.x MSVC2022 64bit
```

### 3. Запустіть
```
Build → Run (Ctrl+R)
```

✅ Готово!

---

## Linux

### 1. Встановіть залежності
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install qt6-base-dev pciutils build-essential

# Опціонально для NVIDIA VRAM
sudo apt install nvidia-utils

# Fedora
sudo dnf install qt6-qtbase-devel pciutils gcc-c++

# Arch
sudo pacman -S qt6-base pciutils
```

### 2. Збірка
```bash
# Через qmake
qmake6 hwinfo.pro
make

# Або через CMake
mkdir build && cd build
cmake ..
cmake --build .
```

### 3. Запуск
```bash
./hwinfo
```

✅ Готово!

---

## Приклад виводу

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
  VRAM Використання: 30.0%

===========================
```

---

## Використання в коді

```cpp
#include "HardwareInfoProvider.h"

HardwareInfoProvider hw;

// Вся інформація
qDebug().noquote() << hw.getAllSystemInfo();

// Або окремо
qDebug() << "CPU:" << hw.getCPUName();
qDebug() << "RAM:" << hw.formatBytes(hw.getTotalRAM());
qDebug() << "GPU:" << hw.getGPUName();
```

---

## Troubleshooting

### Linux: "GPU information not available"
```bash
sudo apt install pciutils
```

### Linux NVIDIA: VRAM = 0
```bash
sudo apt install nvidia-utils
nvidia-smi  # перевірка
```

### Windows: Помилка компіляції
Переконайтесь що встановлено Windows SDK

---

**Все готово! 🎉**
