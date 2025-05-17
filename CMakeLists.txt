cmake_minimum_required(VERSION 3.5)

project(Cornifer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  bs_thread_pool
  GIT_REPOSITORY https://github.com/bshoshany/thread-pool.git
  GIT_TAG v5.0.0 # یا آخرین نسخه
)
FetchContent_MakeAvailable(bs_thread_pool)


include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.15.3
)
FetchContent_MakeAvailable(spdlog)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)


if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        searcherror.h
        logger.h logger.cpp
        customerrors.h
        mainwindow.h
        wildcardmatcher.h
        wildcardmatcher.cpp
        wildcarderror.h
        filematcher.h filematcher.cpp
        concurrentfilesearcher.h concurrentfilesearcher.cpp
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Cornifer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Cornifer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Cornifer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Cornifer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Cornifer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
#///////
target_include_directories(Cornifer PRIVATE ${bs_thread_pool_SOURCE_DIR}/include)
target_include_directories(Cornifer PRIVATE include)
target_link_libraries(Cornifer PRIVATE Threads::Threads spdlog::spdlog)
#target_link_libraries(test_ PRIVATE spdlog::spdlog)
#
target_compile_features(Cornifer PRIVATE cxx_std_17)
set_target_properties(Cornifer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Cornifer)
endif()
