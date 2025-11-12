cmake_minimum_required(VERSION 3.16)
project(hwinfo VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Знайти Qt
find_package(Qt6 REQUIRED COMPONENTS Core)

# Створити виконуваний файл
add_executable(hwinfo
    main.cpp
    HardwareInfoProvider.cpp
    SystemScorer.cpp          # <— додай цю лінію
    HardwareInfoProvider.h
    SystemScorer.h            # <— і цю теж бажано
)

# Лінкування з Qt
target_link_libraries(hwinfo
    Qt6::Core
)

# Windows-specific libraries
if(WIN32)
    target_link_libraries(hwinfo
        dxgi
        wbemuuid
    )
endif()

# Встановлення
install(TARGETS hwinfo
    RUNTIME DESTINATION bin
)
