QT       += core
QT       -= gui

TARGET = hwinfo
CONFIG   += console c++17
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    HardwareInfoProvider.cpp

HEADERS += \
    HardwareInfoProvider.h

# Windows-specific libraries
win32 {
    LIBS += -ldxgi -lwbemuuid
}

# Linux-specific settings
unix:!macx {
    # Нічого додаткового не потрібно
    # Переконайтесь що встановлено: sudo apt install pciutils
}
HEADERS += \
    HardwareInfoProvider.h \
    SystemScorer.h

SOURCES += \
    main.cpp \
    HardwareInfoProvider.cpp \
    SystemScorer.cpp
```

---

## 🎯 **Очікуваний вивід:**
```
=====================================
  Hardware Info Provider v5.0
  With SystemScorer
=====================================

Collecting device information...
Done!

=== ArgentumDevice Structure ===
[... базова інформація ...]

>>> MINING PERFORMANCE <

========================================
  DETAILED SYSTEM PERFORMANCE SCORE
========================================

Total Score: 60.9/100 (Good)

GPU Score: 26.7/100
  Details: Found in database: RX 6600 XT
      Hashrate: 32.0 MH/s
      Efficiency: 0.48 MH/W
      Tier: 3/5

VRAM Score: 90.0/100
  Details: 8+ GB - Great for mining (current requirements)

CPU Score: 100.0/100
  Details: Excellent (16+ cores)
      Frequency: Good (3.0-3.5 GHz) (+5 bonus)

RAM Score: 100.0/100
  Details: 16+ GB - Excellent (more than needed)

Disk Score: 100.0/100
  Details: SSD - Excellent performance (Good: 121 GB free)

Overall Recommendation:
  System is well-suited for mining. Minor upgrades may improve performance.

========================================

>>> AI TRAINING PERFORMANCE <
[... інші оцінки ...]

>>> TASK COMPARISON <

Task                         Score         Rating
-------------------------------------------------------
Mining                        60.9           Good
AI Training                   55.2        Average
Video Rendering               78.5           Good
Gaming                        62.3           Good
General Purpose               70.8           Good
-------------------------------------------------------

Best Use Case: Video Rendering (78.5/100)

>>> EXAMPLE: COMPARING TWO SYSTEMS <

========================================
  SYSTEM COMPARISON
========================================

Component           Your System   RTX 4080 System          Winner
-----------------------------------------------------------------
Total Score                60.9           83.4    RTX 4080 System
GPU                        26.7           83.3    RTX 4080 System
VRAM                       90.0          100.0    RTX 4080 System
CPU                       100.0          100.0                Tie
RAM                       100.0          100.0                Tie
Disk                      100.0          100.0                Tie
-----------------------------------------------------------------

Verdict: RTX 4080 System is better by 22 points

========================================