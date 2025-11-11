#pragma once
#include <QString>
#include <QVector>

struct GPUInfo {
    QString name;
    qint64 vramBytes;
    QString type;
};

struct HardwareInfo {
    int cpuCores = 0;
    int logicalProcessors = 0;
    double cpuFrequencyMHz = 0.0;
    qint64 ramBytes = 0;
    QVector<GPUInfo> gpus;
    QString osName;
    QString osVersion;
};
