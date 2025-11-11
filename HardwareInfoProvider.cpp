#include "HardwareInfoProvider.h"
#include <QSysInfo>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStorageInfo>
#include <QDir>
#include <string>

// Platform-specific includes
#ifdef _WIN32
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <dxgi.h>
// Спробуємо включити нові версії DXGI якщо доступні
#if defined(NTDDI_WIN10) || defined(_WIN32_WINNT_WIN10)
    #include <dxgi1_4.h>
#endif
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "dxgi.lib")
#endif

#ifdef __linux__
#include <sys/sysinfo.h>
#include <unistd.h>
#include <QProcess>
#include <QDir>
#include <QRegularExpression>
#endif

// ========================================
// Конструктор та деструктор
// ========================================

HardwareInfoProvider::HardwareInfoProvider()
{
}

HardwareInfoProvider::~HardwareInfoProvider()
{
}

// ========================================
// Інформація про ОС
// ========================================

QString HardwareInfoProvider::getOSInfo() const
{
    return QSysInfo::prettyProductName();
}

QString HardwareInfoProvider::getKernelVersion() const
{
    return QSysInfo::kernelVersion();
}

QString HardwareInfoProvider::getArchitecture() const
{
    return QSysInfo::currentCpuArchitecture();
}

QString HardwareInfoProvider::getPlatformName() const
{
#ifdef _WIN32
    return "Windows";
#elif defined(__linux__)
    return "Linux";
#else
    return "Unknown";
#endif
}

// ========================================
// Інформація про CPU - Windows
// ========================================

#ifdef _WIN32
QString HardwareInfoProvider::getCPUNameFromRegistry() const
{
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        wchar_t buffer[256];
        DWORD size = sizeof(buffer);
        if (RegQueryValueEx(hKey, L"ProcessorNameString", nullptr, nullptr,
            (LPBYTE)buffer, &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return QString::fromWCharArray(buffer).trimmed();
        }
        RegCloseKey(hKey);
    }
    return "Unknown CPU";
}

int HardwareInfoProvider::getCPUFrequencyFromRegistry() const
{
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD mhz = 0;
        DWORD size = sizeof(mhz);
        if (RegQueryValueEx(hKey, L"~MHz", nullptr, nullptr,
            (LPBYTE)&mhz, &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return static_cast<int>(mhz);
        }
        RegCloseKey(hKey);
    }
    return 0;
}
#endif

// ========================================
// Інформація про CPU - Linux
// ========================================

#ifdef __linux__
QString HardwareInfoProvider::getLinuxCPUInfo() const
{
    QFile cpuInfo("/proc/cpuinfo");
    if (cpuInfo.open(QIODevice::ReadOnly)) {
        QTextStream stream(&cpuInfo);
        QString line;
        while (stream.readLineInto(&line)) {
            if (line.startsWith("model name")) {
                QStringList parts = line.split(":");
                if (parts.size() >= 2) {
                    return parts[1].trimmed();
                }
            }
        }
    }
    return "Unknown CPU";
}
#endif

// ========================================
// Інформація про CPU - Загальні методи
// ========================================

QString HardwareInfoProvider::getCPUName() const
{
#ifdef _WIN32
    return getCPUNameFromRegistry();
#elif defined(__linux__)
    return getLinuxCPUInfo();
#else
    return "Unknown CPU";
#endif
}

int HardwareInfoProvider::getCPUCores() const
{
    return QThread::idealThreadCount();
}

int HardwareInfoProvider::getCPUFrequencyMHz() const
{
#ifdef _WIN32
    return getCPUFrequencyFromRegistry();
#elif defined(__linux__)
    QFile cpuFreq("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
    if (cpuFreq.open(QIODevice::ReadOnly)) {
        QString freq = cpuFreq.readAll().trimmed();
        return freq.toInt() / 1000; // KHz to MHz
    }
    return 0;
#else
    return 0;
#endif
}

double HardwareInfoProvider::getCPUFrequencyGHz() const
{
    int mhz = getCPUFrequencyMHz();
    return mhz > 0 ? mhz / 1000.0 : 0.0;
}

// ========================================
// Інформація про RAM - Linux
// ========================================

#ifdef __linux__
quint64 HardwareInfoProvider::getLinuxTotalRAM() const
{
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return info.totalram * info.mem_unit;
    }
    return 0;
}

quint64 HardwareInfoProvider::getLinuxAvailableRAM() const
{
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return info.freeram * info.mem_unit;
    }
    return 0;
}
#endif

// ========================================
// Інформація про RAM - Загальні методи
// ========================================

quint64 HardwareInfoProvider::getTotalRAM() const
{
#ifdef _WIN32
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    if (GlobalMemoryStatusEx(&statex)) {
        return statex.ullTotalPhys;
    }
    return 0;
#elif defined(__linux__)
    return getLinuxTotalRAM();
#else
    return 0;
#endif
}

quint64 HardwareInfoProvider::getAvailableRAM() const
{
#ifdef _WIN32
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    if (GlobalMemoryStatusEx(&statex)) {
        return statex.ullAvailPhys;
    }
    return 0;
#elif defined(__linux__)
    return getLinuxAvailableRAM();
#else
    return 0;
#endif
}

quint64 HardwareInfoProvider::getUsedRAM() const
{
    quint64 total = getTotalRAM();
    quint64 available = getAvailableRAM();
    return total > available ? total - available : 0;
}

double HardwareInfoProvider::getRAMUsagePercent() const
{
    quint64 total = getTotalRAM();
    if (total == 0) return 0.0;
    
    quint64 used = getUsedRAM();
    return (used * 100.0) / total;
}

// ========================================
// Інформація про GPU - Windows
// ========================================

#ifdef _WIN32
QString HardwareInfoProvider::getWindowsGPUInfo() const
{
    IDXGIFactory* pFactory = nullptr;
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory))))
        return "Not found";

    QStringList gpuList;
    QSet<QString> seenAdapters;

    UINT i = 0;
    IDXGIAdapter* pAdapter = nullptr;

    while (pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC desc;
        if (SUCCEEDED(pAdapter->GetDesc(&desc))) {
            QString gpuName = QString::fromWCharArray(desc.Description).trimmed();
            QString gpuKey = QString("%1_%2_%3")
                .arg(desc.VendorId)
                .arg(desc.DeviceId)
                .arg(gpuName);

            // уникаємо повторів
            if (!seenAdapters.contains(gpuKey)) {
                seenAdapters.insert(gpuKey);

                SIZE_T vramMB = desc.DedicatedVideoMemory / (1024 * 1024);
                QString entry = QString("GPU %1: \"%2\"\n  Total VRAM: %3 MB (\"%4\" GB)")
                    .arg(gpuList.size() + 1)
                    .arg(gpuName)
                    .arg(vramMB)
                    .arg(QString::number(vramMB / 1024.0, 'f', 2));

                gpuList.append(entry);
            }
        }
        pAdapter->Release();
        ++i;
    }

    pFactory->Release();
    return gpuList.join("\n\n");
}
#endif

// ========================================
// Інформація про GPU - Linux
// ========================================

#ifdef __linux__
QString HardwareInfoProvider::getLinuxGPUFromSys() const
{
    QDir drmDir("/sys/class/drm");
    if (drmDir.exists()) {
        QStringList cards = drmDir.entryList(QStringList() << "card*", QDir::Dirs);
        for (const QString &card : cards) {
            if (card.contains("-")) continue;
            
            QString devicePath = QString("/sys/class/drm/%1/device").arg(card);
            QFile vendorFile(devicePath + "/vendor");
            QFile deviceFile(devicePath + "/device");
            
            QString vendor, device;
            
            if (vendorFile.open(QIODevice::ReadOnly)) {
                vendor = QString(vendorFile.readAll()).trimmed();
                vendorFile.close();
            }
            
            if (deviceFile.open(QIODevice::ReadOnly)) {
                device = QString(deviceFile.readAll()).trimmed();
                deviceFile.close();
            }
            
            if (!vendor.isEmpty() || !device.isEmpty()) {
                QString vendorName = vendor;
                if (vendor == "0x8086") vendorName = "Intel";
                else if (vendor == "0x10de") vendorName = "NVIDIA";
                else if (vendor == "0x1002") vendorName = "AMD";
                
                return QString("%1 Graphics (Device: %2)").arg(vendorName, device);
            }
        }
    }
    
    return QString();
}

QString HardwareInfoProvider::getLinuxGPUFromLspci() const
{
    QProcess process;
    process.start("lspci", QStringList() << "-v");
    
    if (!process.waitForFinished(3000)) {
        return QString();
    }
    
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n');
    
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        if (line.contains("VGA compatible controller:", Qt::CaseInsensitive) ||
            line.contains("3D controller:", Qt::CaseInsensitive)) {
            
            int colonPos = line.lastIndexOf(':');
            if (colonPos > 0) {
                QString gpuName = line.mid(colonPos + 1).trimmed();
                
                QString vram;
                for (int j = i + 1; j < qMin(i + 10, lines.size()); ++j) {
                    QString nextLine = lines[j];
                    if (nextLine.contains("Memory at", Qt::CaseInsensitive) && 
                        nextLine.contains("prefetchable", Qt::CaseInsensitive)) {
                        
                        QRegularExpression sizeRegex(R"(\[size=(\d+)([KMG])\])");
                        QRegularExpressionMatch match = sizeRegex.match(nextLine);
                        if (match.hasMatch()) {
                            int size = match.captured(1).toInt();
                            QString unit = match.captured(2);
                            
                            if (unit == "G") {
                                vram = QString(" (VRAM: %1 GB)").arg(size);
                            } else if (unit == "M") {
                                vram = QString(" (VRAM: %1 MB)").arg(size);
                            }
                            break;
                        }
                    }
                }
                
                return gpuName + vram;
            }
        }
    }
    
    return QString();
}

QString HardwareInfoProvider::getLinuxGPUInfo() const
{
    QString gpu = getLinuxGPUFromLspci();
    if (!gpu.isEmpty()) {
        return gpu;
    }
    
    gpu = getLinuxGPUFromSys();
    if (!gpu.isEmpty()) {
        return gpu;
    }
    
    return "GPU information not available";
}
#endif

// ========================================
// Інформація про GPU - Загальні методи
// ========================================

QString HardwareInfoProvider::getGPUInfo() const
{
#ifdef _WIN32
    return getWindowsGPUInfo();
#elif defined(__linux__)
    return getLinuxGPUInfo();
#else
    return "GPU information not available";
#endif
}

QString HardwareInfoProvider::getGPUName() const
{
    QString info = getGPUInfo();
    int vramPos = info.indexOf("(VRAM:");
    if (vramPos > 0) {
        return info.left(vramPos).trimmed();
    }
    return info;
}

quint64 HardwareInfoProvider::getGPUMemoryMB() const
{
#ifdef _WIN32
    IDXGIFactory* pFactory = nullptr;
    quint64 vram = 0;
    
    if (SUCCEEDED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory)))) {
        IDXGIAdapter* pAdapter = nullptr;
        if (pFactory->EnumAdapters(0, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
            DXGI_ADAPTER_DESC desc;
            if (SUCCEEDED(pAdapter->GetDesc(&desc))) {
                vram = desc.DedicatedVideoMemory / 1024 / 1024;
            }
            pAdapter->Release();
        }
        pFactory->Release();
    }
    return vram;
#else
    return 0;
#endif
}

quint64 HardwareInfoProvider::getGPUUsedMemoryMB() const
{
#ifdef _WIN32
    // Windows: спробуємо використати DXGI 1.4 (Windows 10+)
    quint64 usedMemory = 0;
    
    // Перевіряємо чи доступний IDXGIFactory4
    #if defined(NTDDI_WIN10) || defined(_WIN32_WINNT_WIN10)
        // Новіші Windows SDK - використовуємо DXGI 1.4
        typedef HRESULT (WINAPI* PFN_CREATE_DXGI_FACTORY1)(REFIID, void**);
        HMODULE hDxgi = LoadLibraryA("dxgi.dll");
        
        if (hDxgi) {
            PFN_CREATE_DXGI_FACTORY1 pCreateDXGIFactory1 = 
                (PFN_CREATE_DXGI_FACTORY1)GetProcAddress(hDxgi, "CreateDXGIFactory1");
            
            if (pCreateDXGIFactory1) {
                IDXGIFactory1* pFactory1 = nullptr;
                HRESULT hr = pCreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory1);
                
                if (SUCCEEDED(hr) && pFactory1) {
                    // Спробуємо отримати IDXGIFactory4
                    IDXGIFactory4* pFactory4 = nullptr;
                    hr = pFactory1->QueryInterface(__uuidof(IDXGIFactory4), (void**)&pFactory4);
                    
                    if (SUCCEEDED(hr) && pFactory4) {
                        IDXGIAdapter3* pAdapter3 = nullptr;
                        if (pFactory4->EnumAdapters(0, (IDXGIAdapter**)&pAdapter3) != DXGI_ERROR_NOT_FOUND) {
                            DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo;
                            if (SUCCEEDED(pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo))) {
                                usedMemory = memoryInfo.CurrentUsage / 1024 / 1024;
                            }
                            pAdapter3->Release();
                        }
                        pFactory4->Release();
                    }
                    pFactory1->Release();
                }
            }
            FreeLibrary(hDxgi);
        }
    #else
        // Старіші Windows SDK - VRAM usage недоступний
        // Повертаємо 0
        usedMemory = 0;
    #endif
    
    return usedMemory;
    
#elif defined(__linux__)
    // Linux: nvidia-smi для NVIDIA
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
    
    // Linux: AMD через sysfs
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

quint64 HardwareInfoProvider::getGPUFreeMemoryMB() const
{
#ifdef _WIN32
    // Windows: отримуємо вільну VRAM через DXGI 1.4
    quint64 freeMemory = 0;
    
    // Перевіряємо чи доступний IDXGIFactory4
    #if defined(NTDDI_WIN10) || defined(_WIN32_WINNT_WIN10)
        typedef HRESULT (WINAPI* PFN_CREATE_DXGI_FACTORY1)(REFIID, void**);
        HMODULE hDxgi = LoadLibraryA("dxgi.dll");
        
        if (hDxgi) {
            PFN_CREATE_DXGI_FACTORY1 pCreateDXGIFactory1 = 
                (PFN_CREATE_DXGI_FACTORY1)GetProcAddress(hDxgi, "CreateDXGIFactory1");
            
            if (pCreateDXGIFactory1) {
                IDXGIFactory1* pFactory1 = nullptr;
                HRESULT hr = pCreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory1);
                
                if (SUCCEEDED(hr) && pFactory1) {
                    // Спробуємо отримати IDXGIFactory4
                    IDXGIFactory4* pFactory4 = nullptr;
                    hr = pFactory1->QueryInterface(__uuidof(IDXGIFactory4), (void**)&pFactory4);
                    
                    if (SUCCEEDED(hr) && pFactory4) {
                        IDXGIAdapter3* pAdapter3 = nullptr;
                        if (pFactory4->EnumAdapters(0, (IDXGIAdapter**)&pAdapter3) != DXGI_ERROR_NOT_FOUND) {
                            DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo;
                            if (SUCCEEDED(pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo))) {
                                // Budget - це загальна доступна пам'ять для процесу
                                // CurrentUsage - використана пам'ять
                                // Вільна = Budget - CurrentUsage
                                freeMemory = (memoryInfo.Budget - memoryInfo.CurrentUsage) / 1024 / 1024;
                            }
                            pAdapter3->Release();
                        }
                        pFactory4->Release();
                    }
                    pFactory1->Release();
                }
            }
            FreeLibrary(hDxgi);
        }
    #else
        // Старіші Windows SDK - недоступно
        freeMemory = 0;
    #endif
    
    return freeMemory;
    
#elif defined(__linux__)
    // Linux: nvidia-smi для NVIDIA
    QProcess process;
    process.start("nvidia-smi", QStringList() 
        << "--query-gpu=memory.free" 
        << "--format=csv,noheader,nounits");
    
    if (process.waitForFinished(2000)) {
        QString output = process.readAllStandardOutput().trimmed();
        bool ok;
        quint64 free = output.toULongLong(&ok);
        if (ok && free > 0) {
            return free;
        }
    }
    
    // AMD через sysfs
    QDir drmDir("/sys/class/drm");
    if (drmDir.exists()) {
        QStringList cards = drmDir.entryList(QStringList() << "card*", QDir::Dirs);
        for (const QString &card : cards) {
            if (card.contains("-")) continue;
            
            QString memTotalPath = QString("/sys/class/drm/%1/device/mem_info_vram_total").arg(card);
            QString memUsedPath = QString("/sys/class/drm/%1/device/mem_info_vram_used").arg(card);
            
            QFile memTotalFile(memTotalPath);
            QFile memUsedFile(memUsedPath);
            
            if (memTotalFile.open(QIODevice::ReadOnly) && memUsedFile.open(QIODevice::ReadOnly)) {
                QString totalStr = memTotalFile.readAll().trimmed();
                QString usedStr = memUsedFile.readAll().trimmed();
                
                bool okTotal, okUsed;
                quint64 totalBytes = totalStr.toULongLong(&okTotal);
                quint64 usedBytes = usedStr.toULongLong(&okUsed);
                
                if (okTotal && okUsed) {
                    return (totalBytes - usedBytes) / 1024 / 1024;
                }
            }
        }
    }
    
    return 0;
#else
    return 0;
#endif
}

double HardwareInfoProvider::getGPUMemoryUsagePercent() const
{
    quint64 total = getGPUMemoryMB();
    if (total == 0) return 0.0;
    
    quint64 used = getGPUUsedMemoryMB();
    return (used * 100.0) / total;
}

// ========================================
// Інформація про диски - Windows (комбінований метод)
// ========================================

#ifdef _WIN32
QString HardwareInfoProvider::getWindowsDiskType(const QString& drive) const
{
    QString driveLetter = drive;
    driveLetter.remove(":\\");
    driveLetter.remove(":");
    driveLetter = driveLetter.toUpper();

    QString diskType = "Unknown";
    qDebug() << "[DEBUG] Перевіряємо диск:" << driveLetter;

    static QMap<QString, QString> cache;
    if (cache.contains(driveLetter))
        return cache[driveLetter];

    // ---------------------------
    // 1. Ініціалізація COM
    // ---------------------------
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres) && hres != RPC_E_CHANGED_MODE) {
        qDebug() << "[DEBUG] CoInitializeEx не вдалося";
        return diskType;
    }

    hres = CoInitializeSecurity(NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL);

    if (FAILED(hres) && hres != RPC_E_TOO_LATE) {
        qDebug() << "[DEBUG] CoInitializeSecurity не вдалося";
        CoUninitialize();
        return diskType;
    }

    IWbemLocator* pLoc = NULL;
    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);
    if (FAILED(hres)) {
        qDebug() << "[DEBUG] Не вдалося створити WbemLocator";
        CoUninitialize();
        return diskType;
    }

    // ---------------------------
    // 2. Основний запит: MSFT_PhysicalDisk
    // ---------------------------
    IWbemServices* pSvc = NULL;
    hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\Microsoft\\Windows\\Storage"),
        NULL, NULL, 0, NULL, 0, 0, &pSvc);

    if (SUCCEEDED(hres)) {
        CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
            RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

        IEnumWbemClassObject* pEnumerator = NULL;
        QString query = "SELECT FriendlyName, MediaType, BusType FROM MSFT_PhysicalDisk";
        hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t(query.toStdWString().c_str()),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

        if (SUCCEEDED(hres)) {
            IWbemClassObject* pObj = NULL;
            ULONG uReturn = 0;
            while (pEnumerator && pEnumerator->Next(WBEM_INFINITE, 1, &pObj, &uReturn) == S_OK) {
                VARIANT vtName, vtMedia, vtBus;
                VariantInit(&vtName);
                VariantInit(&vtMedia);
                VariantInit(&vtBus);

                pObj->Get(L"FriendlyName", 0, &vtName, 0, 0);
                pObj->Get(L"MediaType", 0, &vtMedia, 0, 0);
                pObj->Get(L"BusType", 0, &vtBus, 0, 0);

                QString name = (vtName.vt == VT_BSTR) ? QString::fromWCharArray(vtName.bstrVal) : "Unknown";
                int mediaType = (vtMedia.vt == VT_I4) ? vtMedia.intVal : -1;
                int busType = (vtBus.vt == VT_I4) ? vtBus.intVal : -1;

                qDebug() << "[DEBUG] Знайдено диск:" << name << "| MediaType:" << mediaType << "| BusType:" << busType;

                switch (mediaType) {
                case 3: diskType = "HDD"; break;
                case 4: diskType = "SSD"; break;
                case 5: diskType = "SSM"; break;
                default:
                    if (busType == 7 || busType == 11)
                        diskType = "Removable";
                    else if (busType == 9)
                        diskType = "External";
                    break;
                }

                if (diskType != "Unknown") {
                    qDebug() << "[DEBUG] Визначено тип через MSFT_PhysicalDisk:" << diskType;
                    VariantClear(&vtName);
                    VariantClear(&vtMedia);
                    VariantClear(&vtBus);
                    pObj->Release();
                    break;
                }

                VariantClear(&vtName);
                VariantClear(&vtMedia);
                VariantClear(&vtBus);
                pObj->Release();
            }
            pEnumerator->Release();
        }
        pSvc->Release();
    }

    // ---------------------------
    // 3. Резервний запит: Win32_DiskDrive (якщо Unknown)
    // ---------------------------
    if (diskType == "Unknown") {
        qDebug() << "[DEBUG] Переходимо до резервного запиту Win32_DiskDrive...";
        IWbemServices* pSvc2 = NULL;
        hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc2);
        if (SUCCEEDED(hres)) {
            CoSetProxyBlanket(pSvc2, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

            QString diskQuery = "SELECT Model, MediaType FROM Win32_DiskDrive";
            IEnumWbemClassObject* pEnum2 = NULL;
            hres = pSvc2->ExecQuery(bstr_t("WQL"), bstr_t(diskQuery.toStdWString().c_str()),
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnum2);

            if (SUCCEEDED(hres)) {
                IWbemClassObject* pDiskObj = NULL;
                ULONG uDiskReturn = 0;
                while (pEnum2 && pEnum2->Next(WBEM_INFINITE, 1, &pDiskObj, &uDiskReturn) == S_OK) {
                    VARIANT vtMedia, vtModel;
                    VariantInit(&vtMedia);
                    VariantInit(&vtModel);

                    pDiskObj->Get(L"MediaType", 0, &vtMedia, 0, 0);
                    pDiskObj->Get(L"Model", 0, &vtModel, 0, 0);

                    QString mediaTypeStr;
                    if (vtMedia.vt == VT_BSTR)
                        mediaTypeStr = QString::fromWCharArray(vtMedia.bstrVal);

                    QString model = (vtModel.vt == VT_BSTR) ? QString::fromWCharArray(vtModel.bstrVal).toUpper() : "";

                    qDebug() << "[DEBUG] Win32_DiskDrive: Model=" << model << "MediaType=" << mediaTypeStr;

                    if (model.contains("SSD") || model.contains("NVME") || model.contains("SOLID STATE"))
                        diskType = "SSD";
                    else if (mediaTypeStr.contains("Fixed", Qt::CaseInsensitive))
                        diskType = "HDD";
                    else if (mediaTypeStr.contains("Removable", Qt::CaseInsensitive))
                        diskType = "Removable";

                    VariantClear(&vtMedia);
                    VariantClear(&vtModel);
                    pDiskObj->Release();

                    if (diskType != "Unknown") break;
                }
                pEnum2->Release();
            }
            pSvc2->Release();
        }
    }

    // ---------------------------
    // 4. Очистка та результат
    // ---------------------------
    pLoc->Release();
    CoUninitialize();

    qDebug() << "[DEBUG] Фінальний тип диску для" << driveLetter << ":" << diskType;

    if (diskType.isEmpty())
        return "Unknown";

    cache[driveLetter] = diskType;
    return diskType;
}
#endif


// ========================================
// Інформація про диски - Linux (через lsblk)
// ========================================

#ifdef __linux__
QString HardwareInfoProvider::getLinuxDiskType(const QString& device) const
{
    QString devName = device;
    devName.remove("/dev/");

    static QMap<QString, QString> cache;
    if (cache.contains(devName)) {
        qDebug() << "[DEBUG] Використано кешований тип диску для" << devName << ":" << cache[devName];
        return cache[devName];
    }

    devName.replace(QRegularExpression("\\d+$"), ""); // видаляємо номер партиції

    qDebug() << "[DEBUG] Перевіряємо диск:" << devName;

    QProcess process;
    process.start("lsblk", { "-d", "-o", "NAME,ROTA,TRAN,TYPE,MODEL" });
    if (!process.waitForFinished(1500)) {
        qDebug() << "[DEBUG] lsblk не відповідає";
        return "Unknown";
    }

    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    QString diskType = "Unknown";

    for (const QString& line : lines) {
        if (line.startsWith(devName)) {
            QStringList parts = line.simplified().split(' ');
            if (parts.size() < 5) continue;

            QString name = parts.at(0);
            QString rota = parts.at(1);
            QString tran = parts.at(2).toLower();
            QString type = parts.at(3).toLower();
            QString model = parts.mid(4).join(' ');

            qDebug() << "[DEBUG] lsblk знайдено:" << name << "| ROTA:" << rota << "| TRAN:" << tran << "| TYPE:" << type << "| MODEL:" << model;

            if (type == "rom" || type == "loop") {
                diskType = "Removable";
            }
            else if (tran.contains("usb") || tran.contains("thunderbolt")) {
                diskType = "External";
            }
            else if (rota == "0") {
                diskType = "SSD";
            }
            else if (rota == "1") {
                diskType = "HDD";
            }

            if (diskType != "Unknown") break;
        }
    }

    qDebug() << "[DEBUG] Фінальний тип диску для" << devName << ":" << diskType;

    cache[devName] = diskType;

    return diskType;
}
#endif

// ========================================
// Інформація про диски - Загальні методи
// ========================================

QList<DiskInfo> HardwareInfoProvider::getDisks() const
{
    QList<DiskInfo> disks;
    
    QList<QStorageInfo> volumes = QStorageInfo::mountedVolumes();
    
    for (const QStorageInfo &storage : volumes) {
        // Пропускаємо системні та віртуальні диски
        if (!storage.isValid() || storage.isReadOnly()) {
            continue;
        }
        
#ifdef __linux__
        // На Linux пропускаємо snap, tmpfs, devtmpfs тощо
        QString fsType = QString::fromLatin1(storage.fileSystemType());
        if (fsType == "tmpfs" || fsType == "devtmpfs" || 
            fsType == "squashfs" || fsType == "overlay") {
            continue;
        }
        
        // Пропускаємо /boot, /boot/efi тощо (малі системні партиції)
        QString mount = storage.rootPath();
        if (mount.startsWith("/boot") || mount.startsWith("/sys") || 
            mount.startsWith("/proc") || mount.startsWith("/dev") ||
            mount.startsWith("/run")) {
            continue;
        }
#endif
        
        DiskInfo info;
        info.mountPoint = storage.rootPath();
        info.fileSystem = QString::fromLatin1(storage.fileSystemType());
        info.totalBytes = storage.bytesTotal();
        info.freeBytes = storage.bytesFree();
        info.usedBytes = info.totalBytes - info.freeBytes;
        
        if (info.totalBytes > 0) {
            info.usagePercent = (info.usedBytes * 100.0) / info.totalBytes;
        }
        
        // Визначаємо тип диску (SSD/HDD)
#ifdef _WIN32
        info.type = getWindowsDiskType(info.mountPoint);
#elif defined(__linux__)
        info.type = getLinuxDiskType(storage.device());
#else
        info.type = "Unknown";
#endif
        
        // Модель (спрощено - можна розширити через WMI на Windows)
        info.model = ""; // Поки що залишаємо пустим
        
        disks.append(info);
    }
    
    return disks;
}

quint64 HardwareInfoProvider::getTotalDiskSpace() const
{
    quint64 total = 0;
    QList<DiskInfo> disks = getDisks();
    
    for (const DiskInfo &disk : disks) {
        total += disk.totalBytes;
    }
    
    return total;
}

quint64 HardwareInfoProvider::getUsedDiskSpace() const
{
    quint64 used = 0;
    QList<DiskInfo> disks = getDisks();
    
    for (const DiskInfo &disk : disks) {
        used += disk.usedBytes;
    }
    
    return used;
}

quint64 HardwareInfoProvider::getFreeDiskSpace() const
{
    quint64 free = 0;
    QList<DiskInfo> disks = getDisks();
    
    for (const DiskInfo &disk : disks) {
        free += disk.freeBytes;
    }
    
    return free;
}

double HardwareInfoProvider::getDiskUsagePercent() const
{
    quint64 total = getTotalDiskSpace();
    if (total == 0) return 0.0;
    
    quint64 used = getUsedDiskSpace();
    return (used * 100.0) / total;
}

// ========================================
// Форматування
// ========================================

QString HardwareInfoProvider::formatBytes(quint64 bytes) const
{
    if (bytes == 0) return "0 B";
    
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }
    
    return QString::number(size, 'f', 2) + " " + units[unitIndex];
}

// ========================================
// Отримання всієї інформації
// ========================================

QString HardwareInfoProvider::getAllSystemInfo() const
{
    QString info;
    
    info += "=== Системна інформація ===\n\n";
    
    info += "Платформа: " + getPlatformName() + "\n\n";
    
    // ОС
    info += "Операційна система:\n";
    info += "  Назва: " + getOSInfo() + "\n";
    info += "  Ядро: " + getKernelVersion() + "\n";
    info += "  Архітектура: " + getArchitecture() + "\n\n";
    
    // CPU
    info += "Процесор:\n";
    info += "  Модель: " + getCPUName() + "\n";
    info += "  Ядра: " + QString::number(getCPUCores()) + "\n";
    int cpuFreq = getCPUFrequencyMHz();
    if (cpuFreq > 0) {
        info += QString("  Частота: %1 MHz (%2 GHz)\n")
            .arg(cpuFreq)
            .arg(QString::number(getCPUFrequencyGHz(), 'f', 2));
    } else {
        info += "  Частота: Недоступно\n";
    }
    info += "\n";
    
    // RAM
    quint64 totalRAM = getTotalRAM();
    if (totalRAM > 0) {
        info += "Оперативна пам'ять:\n";
        info += "  Загальна: " + formatBytes(totalRAM) + "\n";
        info += "  Доступна: " + formatBytes(getAvailableRAM()) + "\n";
        info += "  Використано: " + formatBytes(getUsedRAM()) + "\n";
        info += QString("  Використання: %1%\n")
            .arg(QString::number(getRAMUsagePercent(), 'f', 1));
    } else {
        info += "Оперативна пам'ять: Недоступно\n";
    }
    info += "\n";
    
    // GPU
    info += "Відеокарта:\n";
    info += "  Назва: " + getGPUName() + "\n";
    
    quint64 totalVRAM = getGPUMemoryMB();
    quint64 usedVRAM = getGPUUsedMemoryMB();
    quint64 freeVRAM = getGPUFreeMemoryMB();
    
    if (totalVRAM > 0) {
        info += QString("  VRAM Загальна: %1 MB (%2 GB)\n")
            .arg(totalVRAM)
            .arg(QString::number(totalVRAM / 1024.0, 'f', 2));
        
        if (usedVRAM > 0) {
            info += QString("  VRAM Використано: %1 MB (%2 GB)\n")
                .arg(usedVRAM)
                .arg(QString::number(usedVRAM / 1024.0, 'f', 2));
        }
        
        if (freeVRAM > 0) {
            info += QString("  VRAM Вільно: %1 MB (%2 GB)\n")
                .arg(freeVRAM)
                .arg(QString::number(freeVRAM / 1024.0, 'f', 2));
        }
        
        if (usedVRAM > 0) {
            info += QString("  VRAM Використання: %1%\n")
                .arg(QString::number(getGPUMemoryUsagePercent(), 'f', 1));
        }
    }
    info += "\n";
    
    // Диски
    QList<DiskInfo> disks = getDisks();
    if (!disks.isEmpty()) {
        info += "Диски:\n";
        
        for (const DiskInfo &disk : disks) {
            info += QString("  %1 (%2)\n").arg(disk.mountPoint, disk.fileSystem);
            
            if (!disk.type.isEmpty() && disk.type != "Unknown") {
                info += QString("    Тип: %1\n").arg(disk.type);
            }
            
            info += QString("    Розмір: %1\n").arg(formatBytes(disk.totalBytes));
            info += QString("    Вільно: %1 (%2%)\n")
                .arg(formatBytes(disk.freeBytes))
                .arg(QString::number(100.0 - disk.usagePercent, 'f', 1));
            info += QString("    Використано: %1 (%2%)\n")
                .arg(formatBytes(disk.usedBytes))
                .arg(QString::number(disk.usagePercent, 'f', 1));
        }
        
        info += "\n";
        info += "  Всього на дисках:\n";
        info += QString("    Загальний розмір: %1\n").arg(formatBytes(getTotalDiskSpace()));
        info += QString("    Вільно: %1\n").arg(formatBytes(getFreeDiskSpace()));
        info += QString("    Використано: %1 (%2%)\n")
            .arg(formatBytes(getUsedDiskSpace()))
            .arg(QString::number(getDiskUsagePercent(), 'f', 1));
    }
    info += "\n";
    
    info += "===========================";
    
    return info;
}
