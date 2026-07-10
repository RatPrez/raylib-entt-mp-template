file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(server ${SERVER_SOURCES})

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(server PRIVATE
    shared
    EnTT::EnTT
    GameNetworkingSockets::GameNetworkingSockets)
