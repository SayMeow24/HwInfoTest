cmake_minimum_required(VERSION 3.16)
project(hwinfo_console)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hwinfo_console
    src/main.cpp
    src/HardwareInfoProvider.cpp
)

target_include_directories(hwinfo_console PRIVATE
    ${PROJECT_SOURCE_DIR}
)

# DXGI + D3D12 (потрібно для DXCore fallback)
target_link_libraries(hwinfo_console
    PRIVATE
        dxgi
        d3d12
)
