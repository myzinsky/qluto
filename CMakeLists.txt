cmake_minimum_required(VERSION 3.14)

# Project Name:
project(qluto VERSION 0.1 LANGUAGES CXX)

# Configuration:
set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ Version")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Add LibIIO:
add_subdirectory(lib/libiio)

# Configure proper linking and finding of libiio:
set(CMAKE_INSTALL_RPATH "@executable_path/lib/libiio")
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

# Search for Qt:
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

add_executable(qluto
    src/pluto.cpp
    src/test.cpp
    src/qml.qrc
)

target_include_directories(qluto
    PRIVATE lib/libiio
)

target_link_libraries(qluto
    PRIVATE iio
    PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick
)

target_compile_definitions(qluto
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(qluto
)

set_target_properties(qluto PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

############

#if(QT_VERSION_MAJOR EQUAL 6)
#    qt_import_qml_plugins(qml-test)
#    qt_finalize_executable(qml-test)
#endif()