# 🔧 Швидке виправлення помилки компіляції

## Проблема
```
error: C2065: 'IDXGIFactory4': undeclared identifier
```

Це означає що у вас старіша версія Windows SDK, яка не підтримує DXGI 1.4.

---

## ✅ Рішення 1: Оновити файли (РЕКОМЕНДОВАНО)

Я вже виправив код! Просто **перезавантажте оновлені файли**:

1. [HardwareInfoProvider.cpp](HardwareInfoProvider.cpp) - ⭐ ОНОВЛЕНО
2. [HardwareInfoProvider.h](HardwareInfoProvider.h)

Тепер код автоматично визначає версію Windows SDK і використовує відповідні API.

---

## ✅ Рішення 2: Оновити Windows SDK

### Для Visual Studio 2022:

1. Відкрийте **Visual Studio Installer**
2. Натисніть **Modify** для Visual Studio 2022
3. Виберіть вкладку **Individual components**
4. Знайдіть і встановіть:
   - ✅ **Windows 10 SDK (10.0.19041.0)** або новіший
   - ✅ **Windows 11 SDK (10.0.22621.0)** (рекомендовано)

5. Натисніть **Modify** і дочекайтесь встановлення

### Для Qt Creator:

1. Tools → Options → Kits
2. Виберіть ваш Kit
3. Змініть **Windows SDK** на новішу версію
4. Apply → OK

---

## ✅ Рішення 3: Використати сумісний код

Якщо не можете оновити SDK, ось код який **гарантовано працює** на всіх версіях:

### Замініть метод `getGPUUsedMemoryMB()` на:

```cpp
quint64 HardwareInfoProvider::getGPUUsedMemoryMB() const
{
#ifdef _WIN32
    // Для старих Windows SDK просто повертаємо 0
    // VRAM usage доступний тільки на Windows 10+ з новим SDK
    return 0;
    
#elif defined(__linux__)
    // Linux код залишається без змін
    QProcess process;
    process.start("nvidia-smi", QStringList() 
        << "--query-gpu=memory.used" 
        << "--format=csv,noheader,nounits");
    
    if (process.waitForFinished(2000)) {
        QString output = process.readAllStandardOutput().trimmed();
        bool ok;
        quint64 used = output.toULongLong(&ok);
        if (ok && used > 0) {
            return used;
        }
    }
    
    // AMD через sysfs
    QDir drmDir("/sys/class/drm");
    if (drmDir.exists()) {
        QStringList cards = drmDir.entryList(QStringList() << "card*", QDir::Dirs);
        for (const QString &card : cards) {
            if (card.contains("-")) continue;
            
            QString memUsedPath = QString("/sys/class/drm/%1/device/mem_info_vram_used").arg(card);
            QFile memUsedFile(memUsedPath);
            if (memUsedFile.open(QIODevice::ReadOnly)) {
                QString content = memUsedFile.readAll().trimmed();
                bool ok;
                quint64 bytes = content.toULongLong(&ok);
                if (ok && bytes > 0) {
                    return bytes / 1024 / 1024;
                }
            }
        }
    }
    
    return 0;
#else
    return 0;
#endif
}
```

**Примітка:** З цим варіантом на Windows буде показувати 0 для використаної VRAM, але все інше працюватиме.

---

## 🎯 Що відбувається?

### Версії DXGI:

| Версія | Windows | Функціонал |
|--------|---------|------------|
| DXGI 1.0 | Vista+ | Базовий GPU info |
| DXGI 1.1 | 7+ | EnumAdapters1 |
| DXGI 1.2 | 8+ | Більше інфо |
| DXGI 1.3 | 8.1+ | Ще більше |
| DXGI 1.4 | 10+ | **QueryVideoMemoryInfo** ⭐ |

**VRAM Usage** потребує DXGI 1.4 (Windows 10+).

---

## 🚀 Після виправлення

Перекомпілюйте проект:

### Qt Creator:
```
Build → Rebuild All
Ctrl + B
```

### Командний рядок:
```cmd
nmake clean
qmake hwinfo.pro
nmake
```

---

## ✅ Перевірка

Після компіляції ви побачите:

**На Windows 10/11 з новим SDK:**
```
VRAM Використано: 3584 MB (3.50 GB)  ✅
VRAM Використання: 35.0%              ✅
```

**На старіших версіях:**
```
VRAM Використано: 0 MB                ⚠️
```

Це нормально - просто недоступно на старих Windows.

---

## 📚 Детальніше

Дивіться оновлений [README.md](README.md) - там тепер описано всі нюанси сумісності.

---

**Версія:** 3.1 (Compatibility Fix)  
**Дата:** Листопад 2025
