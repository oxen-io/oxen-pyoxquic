cmake_minimum_required(VERSION 3.13...3.25)
project(pyoxquic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(FORCE_PYBIND11_SUBMODULE "Force downloading pybind11 as a submodule" OFF)
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

find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
find_package(PkgConfig REQUIRED)

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
    
    include(FetchContent)
    
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG 0bd8896a4010f2d91b2340570c24fa08606ec406    # release v2.10.3
    )

    FetchContent_MakeAvailable(pybind11)
endif()

python_add_library(oxquic
    MODULE
    src/address.cpp
    src/connection.cpp
    src/endpoint.cpp
    src/network.cpp
)

target_link_libraries(oxquic PRIVATE pybind11::headers)

if (INSTALL_OXQUIC)
    install(TARGETS oxquic DESTINATION pyoxquic)
endif()
