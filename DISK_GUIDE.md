# 💾 Disk Information Guide

## Що можна отримати про диски

### ✅ Реалізовано

| Параметр | Windows | Linux | Опис |
|----------|---------|-------|------|
| **Список дисків** | ✅ | ✅ | C:\, D:\, /, /home |
| **Точка монтування** | ✅ | ✅ | Де диск підключений |
| **Файлова система** | ✅ | ✅ | NTFS, ext4, xfs, btrfs |
| **Розмір (загальний)** | ✅ | ✅ | Повний розмір диску |
| **Вільне місце** | ✅ | ✅ | Доступно для запису |
| **Використане місце** | ✅ | ✅ | Зайнято файлами |
| **Відсоток використання** | ✅ | ✅ | % зайнятого місця |
| **Тип диску** | ✅ | ✅ | SSD чи HDD |

**Легенда:**
- ✅ Повна підтримка

---

## 💻 Використання

### Базовий приклад

```cpp
#include "HardwareInfoProvider.h"

HardwareInfoProvider hw;

// Загальна інформація про всі диски
qDebug() << "Total:" << hw.formatBytes(hw.getTotalDiskSpace());
qDebug() << "Free:" << hw.formatBytes(hw.getFreeDiskSpace());
qDebug() << "Used:" << hw.formatBytes(hw.getUsedDiskSpace());
qDebug() << "Usage:" << hw.getDiskUsagePercent() << "%";
```

### Детальна інформація про кожен диск

```cpp
HardwareInfoProvider hw;
QList<DiskInfo> disks = hw.getDisks();

for (const DiskInfo &disk : disks) {
    qDebug() << "Mount point:" << disk.mountPoint;
    qDebug() << "Type:" << disk.type;  // SSD, HDD, Unknown
    qDebug() << "File system:" << disk.fileSystem;
    qDebug() << "Total:" << hw.formatBytes(disk.totalBytes);
    qDebug() << "Free:" << hw.formatBytes(disk.freeBytes);
    qDebug() << "Used:" << hw.formatBytes(disk.usedBytes);
    qDebug() << "Usage:" << disk.usagePercent << "%";
    qDebug() << "---";
}
```

### Перевірка вільного місця

```cpp
HardwareInfoProvider hw;

double usage = hw.getDiskUsagePercent();
if (usage > 90.0) {
    qWarning() << "Диски майже заповнені!";
    qWarning() << "Вільно:" << hw.formatBytes(hw.getFreeDiskSpace());
}
```

### Пошук конкретного диску

```cpp
HardwareInfoProvider hw;
QList<DiskInfo> disks = hw.getDisks();

// Шукаємо диск C:\ (Windows)
for (const DiskInfo &disk : disks) {
    if (disk.mountPoint == "C:\\") {
        qDebug() << "C: drive has" << disk.freeBytes << "bytes free";
        break;
    }
}

// Або / (Linux)
for (const DiskInfo &disk : disks) {
    if (disk.mountPoint == "/") {
        qDebug() << "Root has" << hw.formatBytes(disk.freeBytes) << "free";
        break;
    }
}
```

---

## 🔍 Як визначається тип диску

### Windows
```
✅ Використовує WMI (Windows Management Instrumentation)

Процес:
1. Знаходить фізичний диск для логічного диска (C:, D:)
2. Запитує Win32_DiskDrive → MediaType
3. Перевіряє Model на ключові слова (SSD, NVMe, Solid State)
4. Визначає тип: SSD, HDD, External, Removable

Типи MediaType:
- "Fixed hard disk media" + Model містить "SSD" = SSD
- "Fixed hard disk media" без "SSD" = HDD
- "External hard disk media" = External
- "Removable Media" = Removable
```

### Linux
```
✅ Читає /sys/block/{device}/queue/rotational

0 = SSD (не обертається)
1 = HDD (обертається)
```

**Приклад:**
```bash
# Перевірка вручну
cat /sys/block/sda/queue/rotational
# 0 - SSD
# 1 - HDD
```

---

## 📊 Приклад виводу

### Windows
```
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
```

### Linux
```
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
```

---

## 🎯 Use Cases

### 1. Моніторинг місця
```cpp
QTimer *timer = new QTimer();
connect(timer, &QTimer::timeout, []() {
    HardwareInfoProvider hw;
    double usage = hw.getDiskUsagePercent();
    qDebug() << "Disk usage:" << usage << "%";
    
    if (usage > 95.0) {
        // Критично мало місця!
    }
});
timer->start(60000); // Кожну хвилину
```

### 2. Вибір диску для збереження
```cpp
HardwareInfoProvider hw;
QList<DiskInfo> disks = hw.getDisks();

DiskInfo bestDisk;
quint64 maxFree = 0;

for (const DiskInfo &disk : disks) {
    if (disk.freeBytes > maxFree) {
        maxFree = disk.freeBytes;
        bestDisk = disk;
    }
}

qDebug() << "Best disk for saving:" << bestDisk.mountPoint;
qDebug() << "Free space:" << hw.formatBytes(bestDisk.freeBytes);
```

### 3. Інформація про систему
```cpp
HardwareInfoProvider hw;
QList<DiskInfo> disks = hw.getDisks();

int ssdCount = 0;
int hddCount = 0;

for (const DiskInfo &disk : disks) {
    if (disk.type == "SSD") ssdCount++;
    else if (disk.type == "HDD") hddCount++;
}

qDebug() << "System has" << ssdCount << "SSDs and" << hddCount << "HDDs";
```

---

## ⚠️ Важливі примітки

### Linux
- **tmpfs, devtmpfs автоматично фільтруються** - це віртуальні файлові системи
- **snap, squashfs фільтруються** - це loop пристрої
- **/boot, /boot/efi пропускаються** - малі системні партиції

### Windows
- **CD-ROM пропускається** - тільки жорсткі диски
- **Мережеві диски включені** - якщо примонтовані

### Загальне
- `QStorageInfo` - Qt клас для роботи з дисками
- Працює на всіх підтримуваних платформах Qt
- Автоматично оновлюється при виклику

---

## 🔧 Покращення (майбутнє)

### Що можна додати:

1. **Windows тип диску через WMI**
```cpp
// Win32_DiskDrive MediaType
// 3 = HDD, 4 = SSD, 5 = SCM
```

2. **Модель диску**
```cpp
// Windows: Win32_DiskDrive Model
// Linux: /sys/block/{dev}/device/model
```

3. **Серійний номер**
```cpp
// Windows: Win32_PhysicalMedia SerialNumber
// Linux: smartctl -i /dev/sda
```

4. **SMART статус**
```cpp
// Потребує libatasmart або smartctl
```

5. **Температура**
```cpp
// Windows: WMI Temperature
// Linux: hddtemp або smartctl
```

---

## 📝 Структура DiskInfo

```cpp
struct DiskInfo {
    QString mountPoint;      // C:\, D:\, /, /home
    QString fileSystem;      // NTFS, ext4, xfs, btrfs
    QString type;            // SSD, HDD, Unknown
    QString model;           // (поки не реалізовано)
    quint64 totalBytes;      // Загальний розмір
    quint64 freeBytes;       // Вільне місце
    quint64 usedBytes;       // Використано
    double usagePercent;     // Відсоток використання
};
```

---

## 🐛 Troubleshooting

### Linux: Не показує тип диску
```bash
# Перевірте чи існує файл
ls -la /sys/block/sda/queue/rotational

# Якщо немає - старе ядро або несправний драйвер
uname -r  # Версія ядра
```

### Windows: Показує Unknown
```
Можливі причини:
1. WMI недоступний (рідко)
2. Відсутні права адміністратора (для деяких систем)
3. Диск не має стандартного MediaType

Рішення:
- Запустіть від адміністратора
- Перевірте чи працює служба "Windows Management Instrumentation"
```

### Диск не відображається
```cpp
// Перевірте чи диск змонтований
QList<QStorageInfo> all = QStorageInfo::mountedVolumes();
for (const QStorageInfo &vol : all) {
    qDebug() << vol.rootPath() << vol.fileSystemType();
}
```

---

**Версія:** 1.0  
**Дата:** Листопад 2025
