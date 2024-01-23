cmake_minimum_required(VERSION 3.13...3.25)
project(pyoxquic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(FORCE_PYBIND11_SUBMODULE "Force downloading pybind11 as a submodule" OFF)
option(FORCE_LIBQUIC_SUBMODULE "Force downloading libquic as as submodule" OFF)
option(INSTALL_OXQUIC "Install pyoxquic as a python module" OFF)
option(OXQUIC_WARN_ERRORS "Treat warnings as errors" OFF)

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    set(OXQUIC_WARN_ERRORS ON)
endif()

set(warning_flags -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-function -Werror=vla -Wdeprecated-declarations)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND warning_flags -Wno-unknown-warning-option)
endif()
if (OXQUIC_WARN_ERRORS)
    list(APPEND warning_flags -Werror)
endif()

include(FetchContent)
find_package(PkgConfig REQUIRED)
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

if (NOT FORCE_PYBIND11_SUBMODULE)
    find_package(pybind11 2.10.3 CONFIG REQUIRED)

    if (TARGET pybind11::pybind11)
        message(STATUS "Found system pybind11!")
    else()
        message(STATUS "Could not find system pybind11!")
        set(FORCE_PYBIND11_SUBMODULE ON)
    endif()
endif()

# check again in case find_package failed
if (FORCE_PYBIND11_SUBMODULE)
    message(STATUS "Using pybind11 submodule!")

    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG 0bd8896a4010f2d91b2340570c24fa08606ec406    # release v2.10.3
    )

    FetchContent_MakeAvailable(pybind11)
endif()

if (NOT FORCE_LIBQUIC_SUBMODULE)
    pkg_check_modules(LIBQUIC libquic>=0.0.5 IMPORTED_TARGET)

    if (LIBQUIC_FOUND)
        message(STATUS "Found system libquic!")
        add_library(quic alias PkgConfig::LIBQUIC)
    else()
        message(STATUS "Could not find system libquic!")
        set(FORCE_LIBQUIC_SUBMODULE ON)
    endif()
endif()

# check againin case find_package failed
if (FORCE_LIBQUIC_SUBMODULE)
    message(STATUS "Using libquic submodule!")

    if (NOT EXISTS ${PROJECT_SOURCE_DIR}/oxen-libquic/CMakeLists.txt)
        message(FATAL_ERROR "Couldn't find oxen-libquic submodule; perhaps you need to `git submodule update --init --recursive`?")
    endif()

    set(LIBQUIC_WARN_DEPRECATED OFF CACHE BOOL "")
    add_subdirectory(oxen-libquic)
endif()

pybind11_add_module(oxquic
    src/oxquic.cpp

    src/address.cpp
    src/btreqstream.cpp
    src/callbacks.cpp
    src/connection.cpp
    src/crypto.cpp
    src/endpoint.cpp
    src/network.cpp
    src/stream.cpp
    src/tls.cpp
)

target_link_libraries(oxquic PRIVATE quic pybind11::headers)
set_target_properties(oxquic PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON)

if (INSTALL_OXQUIC)
    install(TARGETS oxquic DESTINATION pyoxquic)
endif()
