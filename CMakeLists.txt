cmake_minimum_required(VERSION 3.14)

# Project Name:
project(qluto VERSION 0.1 LANGUAGES CXX)

# Build non-cmake libraries (i.e. Liquid):
execute_process(COMMAND sh build_liquid-dsp.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib
)

# Configuration:
set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ Version")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Add LibIIO:
add_subdirectory(lib/libiio)
find_library(LIBIIO_LIBRARIES iio "${CMAKE_CURRENT_LIST_DIR}/build/lib/libiio")
find_path(LIBIIO_INCLUDEDIR iio.h "${CMAKE_CURRENT_LIST_DIR}/lib/libiio")

# Add AD9361
add_subdirectory(lib/libad9361-iio)

# Search for Qt:
find_package(QT NAMES Qt6 Qt5 COMPONENTS Qml Core Quick Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Qml Core Quick Widgets REQUIRED)

add_executable(qluto
    src/main.cpp
    src/pluto.cpp
    src/waterfall.cpp
    src/dsp.cpp
    src/qml.qrc
)

target_include_directories(qluto
    PRIVATE lib/libiio
    PRIVATE lib/libad9361-iio
    PRIVATE lib/liquid-dsp/include
)

target_link_libraries(qluto
    PRIVATE iio
    PRIVATE ad9361
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/build/lib/liquid-dsp/lib/libliquid.ar
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    PRIVATE Qt${QT_VERSION_MAJOR}::Quick
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
)

target_compile_definitions(qluto
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)

set_target_properties(qluto PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER qluto.dl9mj.de
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)