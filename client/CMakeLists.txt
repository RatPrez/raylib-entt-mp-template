file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(client ${CLIENT_SOURCES})

target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(client PRIVATE
    shared
    raylib
    EnTT::EnTT
    GameNetworkingSockets::GameNetworkingSockets)
