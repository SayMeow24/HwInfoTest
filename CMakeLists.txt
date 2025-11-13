cmake_minimum_required(VERSION 3.16)

# Назва проєкту
project(hwinfo_ui VERSION 1.0 LANGUAGES CXX)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Забороняємо Qt Creator автоматично використовувати VCPKG
unset(CMAKE_TOOLCHAIN_FILE CACHE)
set(CMAKE_TOOLCHAIN_FILE "" CACHE STRING "" FORCE)

# Вмикаємо автоматичний MOC/UIC/RCC — ОБОВ’ЯЗКОВО ДЛЯ UI
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Знаходимо Qt 6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Створюємо виконуваний файл
qt_add_executable(hwinfo_ui
    main_ui.cpp
    MainWindow.cpp
    MainWindow.h
    HardwareInfoProvider.cpp
    HardwareInfoProvider.h
)

# Лінкуємо Qt-модулі
target_link_libraries(hwinfo_ui PRIVATE
    Qt6::Core
    Qt6::Widgets
)

# Лінки для Windows
if(WIN32)
    target_link_libraries(hwinfo_ui PRIVATE
        dxgi
        wbemuuid
    )
endif()

# Інсталяція (не обов'язково)
install(TARGETS hwinfo_ui
    RUNTIME DESTINATION bin
)
