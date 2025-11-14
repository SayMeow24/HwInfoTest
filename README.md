# HwInfoTest — Cross-Platform Hardware Information Scanner

<p align="center">
  <img src="https://img.shields.io/badge/OS-Windows%20%7C%20Linux-blue?style=for-the-badge">
  <img src="https://img.shields.io/badge/Language-C%2B%2B17-green?style=for-the-badge">
  <img src="https://img.shields.io/badge/Build-CMake-orange?style=for-the-badge">
  <img src="https://img.shields.io/badge/License-MIT-purple?style=for-the-badge">
  <img src="https://img.shields.io/badge/Status-Stable-success?style=for-the-badge">
</p>

---

# HwInfoTest — Cross-Platform Hardware Information Scanner

HwInfoTest is a lightweight C++17 system scanner that collects detailed hardware information on **Windows** and **Linux**.  
It prints a clean, structured console report and generates a JSON file with full system details.

---

## Features

### Cross-platform  
Supports Windows (AMD/NVIDIA/Intel) and Linux (AMD/NVIDIA/Intel)

### Accurate VRAM Detection  
- Windows AMD → VRAM_Free = Budget, VRAM_Used = Total − Budget  
- Windows NVIDIA → DXGI real usage  
- Linux AMD → `/sys/class/drm`  
- Linux NVIDIA → `nvidia-smi`

### Automatic JSON Export  
Saved next to the executable as `argentum_device.json`.

### Clean Architecture  
- HardwareInfoProvider — hardware scanning  
- ArgentumDevice — unified structure  
- main.cpp — printing + saving JSON  

---

## Windows Build Instructions

```bat
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_TOOLCHAIN_FILE=E:/racoon/projects/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

---

## Linux Build Instructions

```bash
sudo apt install g++ cmake pciutils mesa-utils
sudo apt install nvidia-utils-535  # NVIDIA only

mkdir build
cd build
cmake ..
make -j$(nproc)
```

---

## Example Console Output

```
=== ArgentumDevice Structure ===
Platform: Windows
OS:
  Name: Windows 6.2
  Kernel: 6.2.9200
  Architecture: x86_64
CPU:
  Model: AMD Ryzen 7 5700X3D
  Cores: 16
  Frequency: 3200 MHz
RAM:
  Total: 32649 MB
  Available: 17770 MB
  Used: 14879 MB
GPU:
  GPU 1: AMD Radeon RX 6600 XT
    VRAM Total: 8147 MB
    VRAM Used:  768 MB
    VRAM Free:  7379 MB
```

---

## Example JSON

```json
{
  "platform": "Windows",
  "os": "Windows 6.2",
  "os_kernel": "6.2.9200",
  "os_arch": "x86_64",
  "cpu_model": "AMD Ryzen 7 5700X3D",
  "cpu_cores": 16,
  "cpu_frequency_mhz": 3200,
  "ram_mb": 32649,
  "ram_available_mb": 17770,
  "ram_used_mb": 14879,
  "gpus": [
    {
      "model": "AMD Radeon RX 6600 XT",
      "vram_mb": 8147,
      "vram_used_mb": 768,
      "vram_free_mb": 7379
    }
  ]
}
```

---

## VRAM Logic Summary

### Windows AMD  
DXGI reports Budget as Free VRAM → Used = Total - Budget  
### Windows NVIDIA  
DXGI reports true usage  
### Linux AMD  
Uses `mem_info_vram_used`  
### Linux NVIDIA  
Uses `nvidia-smi`  

---

## License
MIT License.
