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
