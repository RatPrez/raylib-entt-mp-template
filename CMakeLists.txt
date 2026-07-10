cmake_minimum_required(VERSION 3.16)
project(template_mp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG        v3.15.0
)
FetchContent_MakeAvailable(entt)

set(GNS_ROOT "/opt/gns" CACHE PATH "GameNetworkingSockets install prefix")

if(DEFINED ENV{GNS_ROOT})
    set(GNS_ROOT "$ENV{GNS_ROOT}")
endif()

find_library(GNS_LIB
    NAMES GameNetworkingSockets GameNetworkingSockets_s
    PATHS "${GNS_ROOT}/lib" "${GNS_ROOT}/lib64"
    NO_DEFAULT_PATH)

if(GNS_LIB)
    add_library(GameNetworkingSockets::GameNetworkingSockets SHARED IMPORTED)
    set_target_properties(GameNetworkingSockets::GameNetworkingSockets PROPERTIES
        IMPORTED_LOCATION "${GNS_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${GNS_ROOT}/include/GameNetworkingSockets")
    message(STATUS "GameNetworkingSockets: ${GNS_LIB}")
else()
    find_package(GameNetworkingSockets REQUIRED)
    message(STATUS "GameNetworkingSockets: found via find_package")
endif()

add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(client)
