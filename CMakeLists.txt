cmake_minimum_required(VERSION 3.50)

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

set(PROJECT_NAME reflection CXX)
project(${PROJECT_NAME})

set(HEADER_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")

include_directories(AFTER ${HEADER_ROOT})

# libclang
set(LIBCLANG_ROOT "D:/Programs/LLVM")
set(LIBCLANG_INCLUDE ${LIBCLANG_ROOT}/include)
set(LIBCLANG_LIB ${LIBCLANG_ROOT}/lib)
include_directories(AFTER ${LIBCLANG_INCLUDE})
link_directories(AFTER ${LIBCLANG_LIB})
add_library(LIBCLANG MODULE IMPORTED)
set_target_properties(LIBCLANG PROPERTIES IMPORTED_LOCATION_DEBUG "libclang.lib" IMPORTED_LOCATION_RELEASE "libclang.lib")
########################################################################
# Sources
set(HEADERS
    "${HEADER_ROOT}/ast.h"
    "${HEADER_ROOT}/json.hpp"
    "${HEADER_ROOT}/compile_commands.h"
)

set(SOURCES
    "${SOURCE_ROOT}/ast.cpp"
    "${SOURCE_ROOT}/compile_commands.cpp"
    "${SOURCE_ROOT}/main.cpp"
)

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

set(FILES ${HEADERS} ${SOURCES})

add_executable(${PROJECT_NAME} ${FILES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")

if(MSVC)
    set(DEFAULT_CXX_FLAGS "/DWIN32 /D_WINDOWS /D_MSBC /W4 /WX- /nologo /fp:precise /arch:AVX2 /Zc:wchar_t /TP /Gd /utf-8")
    if("1800" VERSION_LESS MSVC_VERSION)
        set(DEFAULT_CXX_FLAGS "${DEFAULT_CXX_FLAGS} /EHsc")
    endif()

    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1 /Gy /GR- /GS /Gm-")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /GL /GR- /DNDEBUG")
    target_link_libraries(${PROJECT_NAME} PRIVATE LIBCLANG)

elseif(UNIX)
    set(DEFAULT_CXX_FLAGS "-Wall -O0 -g -std=c++23 -std=gnu++23 -march=x86-64-v3")
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
elseif(APPLE)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        OUTPUT_NAME_DEBUG "${PROJECT_NAME}" OUTPUT_NAME_RELEASE "${PROJECT_NAME}"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

