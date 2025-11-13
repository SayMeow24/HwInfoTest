cmake_minimum_required(VERSION 3.16)

project(hwinfo_console VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hwinfo_console
    main.cpp
    HardwareInfoProvider.cpp
    HardwareInfoProvider.h
)

if (WIN32)
    # WMI / COM / DXGI
    target_link_libraries(hwinfo_console PRIVATE
        dxgi
        wbemuuid
        ole32
    )
endif()
