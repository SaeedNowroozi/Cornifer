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

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

include_directories(${PROJECT_SOURCE_DIR}/inc)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
     src/*.cpp
     gui/*.cpp
     inc/*.h
)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
add_executable(UnitTests
    tests/test_wildcardmatcher.cpp
        src/wildcardmatcher.cpp
        src/filematcher.cpp
        src/logger.cpp
        src/concurrentfilesearcher.cpp
#       scr/filematcher.cpp
  # ${TEST_SOURCES}
  #  ${PROJECT_SOURCES}
    )
target_link_libraries(UnitTests PRIVATE gtest_main spdlog::spdlog Threads::Threads)
target_include_directories(UnitTests PRIVATE ${PROJECT_SOURCE_DIR}/inc)

enable_testing()
include(GoogleTest)
gtest_discover_tests(UnitTests)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Cornifer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

else()
    if(ANDROID)
        add_library(Cornifer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Cornifer
            ${SOURCES}
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
target_link_libraries(UnitTests PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    gtest_main
    spdlog::spdlog
    Threads::Threads
)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
target_link_libraries(UnitTests PRIVATE Qt${QT_VERSION_MAJOR}::Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
target_link_libraries(UnitTests PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(UnitTests PRIVATE ${bs_thread_pool_SOURCE_DIR}/include)
target_link_libraries(UnitTests PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    gtest_main
    spdlog::spdlog
    Threads::Threads
)



target_compile_features(Cornifer PRIVATE cxx_std_17)
set_target_properties(Cornifer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Cornifer)
endif()
